/*------------------------------------------------------------------------------

                        Database Manipulator

                Low Level Access to functions to a Database
    JCR 4.2004
------------------------------------------------------------------------------*/

#include "dbManipulator.h"

#include "dataViewForm.h"
#include "gnFieldQuery.h"

#include "genAppRes.h"


#ifndef __GEN_APP_RESOURCE_INCLUDE_FILE__
#error  Please create a GenAppRes.H file including the AppName.RSH & RSH files
#error  Then include dataView.RSS in your application resource file!
#endif

#include <eikenv.h>

_LIT( K_selections_extension,".SEL");
_LIT( K_selections_identifier,"[QuodDB SELECTION FILE]" );

#define QUODDB_CURRENT_SEL_FILE_VERSION (1)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

database_manipulator_c::database_manipulator_c()
{
    database = 0;
    file = 0;

    heading = 0;
    showing = 0;

    use_filters = false;

    store_preferences = true;
}

database_manipulator_c::~database_manipulator_c()
{
    bookmarks.Close();
    visible_columns.Close();

    reset_filters();

    filters.Close();
}

bool database_manipulator_c::is_visible( dbm_field_c *field )
{
    if( !visible_columns.Count() )
            return ( true );

    if( field->is_hidden() )
            return ( false );            

    for( int n = 0; n < visible_columns.Count(); n++ )
    {
        if( field == visible_columns[n] )
        {
            return ( true );
        }
    }

    return ( false );

}

bool database_manipulator_c::prepare_for_db( dbm_master_c *_database, dbm_file_c *_file )
{
    database = _database;
    file = _file;

    return ( true );
}

void database_manipulator_c::reset()
{
    use_heading = false;
    heading_field_idx = -1;
    number_of_keys = 0;
    heading = 0;
    showing = 0;
    last_sort = 0;

    reset_filters();

}

bool database_manipulator_c::live( bool _edit_ , bool _use_filters_ )
{
    is_ok = false;

    editable = _edit_;
    use_filters = _use_filters_;

    reset();

    visible_columns.Reset();

    if( !file )
            return ( false );

    import_export.set_file( file );

    bool cfg_loaded = false;

    if( store_preferences )
                cfg_loaded = load();

    for( int n = 0; n < file->n_fields(); n++ )
    {
        if( file->get_field(n)->is_hidden() )
                    continue;

        dbm_field_c *ff = file->get_field( n );

        if( ff->is_key() )
                number_of_keys++;

        if( !cfg_loaded )
                visible_columns.Append( ff );
    }

    file->open( editable ? false : true ); /*Read only*/

    //file->get_table().SetNoIndex(); // Ordena por ordem de inserção, de futuro pode usar mais chaves ...
    sort_by( last_sort );

    is_ok = true;

    return ( true );
}

bool database_manipulator_c::die()
{
//    CEikonEnv::Static()->InfoWinL( _L("Error"),_L("DBMANIPULATOR will dye") );

    if( !file || !is_ok )
            return ( false );

//    CEikonEnv::Static()->InfoWinL( _L("Error"),_L("DBMANIPULATOR will save") );

    if( store_preferences )
            save();

//    CEikonEnv::Static()->InfoWinL( _L("Error"),_L("DBMANIPULATOR will close") );
    file->close();

//    CEikonEnv::Static()->InfoWinL( _L("Error"),_L("DBMANIPULATOR will bye") );
    return ( true );
}

void database_manipulator_c::reset_filters()
{
    for( int n = 0; n < filters.Count(); n++ )
            delete filters[n];

    filters.Reset();

}

CDesCArrayFlat *database_manipulator_c::reload_as_array( int active_visible_column )
{
    bookmarks.Reset();

    CDesCArrayFlat *tArray = new (ELeave) CDesCArrayFlat(100);

    if( !file->n_fields() )
    {
        delete tArray;
        return ( 0 );
    }

    heading = 0;

    if( use_heading && heading_field_idx >= 0 && heading_field_idx < file->n_fields() )
    {
            heading = file->get_field( heading_field_idx );
    }

    if( active_visible_column >= visible_columns.Count() )
    {
            delete tArray;
            return ( 0 );
    }

    showing = visible_columns[ active_visible_column ];

    TChar tab('\t');

    for (file->get_table().FirstL(); file->get_table().AtRow(); file->get_table().NextL())
    {
          //bookmarks.Append( file->get_table().Bookmark() );

          file->get_table().GetL();

          if( use_filters )
          {
              bool filters_ok = true;
              for( int f = 0; f < filters.Count(); f++ )
              {
                    if(!filters[f]->check_on_currrent_record() )
                    {
                        filters_ok = false;
                        break;
                    }
              }

              if( !filters_ok )
                        continue;
          }

          bookmarks.Append( file->get_table().Bookmark() );

          file->sync_links();

          line.Zero();

          //_LIT(KSeparator,"|");
          if( use_heading && heading )
          {
                append_coldata_to_line( heading );
          }

          line.Append( tab );
          append_coldata_to_line( showing );

          tArray->AppendL( line ); // Copy rowText to resultArray
      }

      return tArray;
}

void database_manipulator_c::append_coldata_to_line( dbm_field_c *field )
{
    if( field->is_link() )
    {
        link_details_s *pld = field->get_link_details();

        if( pld->status == true )
        {
            data_item.build_from_db( pld->field_to_show, pld->file );
            line.Append( data_item.get_as_str() );
            return;
        }
    }

    data_item.build_from_db( field, file );
    line.Append( data_item.get_as_str() );
}

bool database_manipulator_c::select_record( int idx )
{
    if( !database || !file || !is_ok )
            return ( false );

    if( idx < 0 || idx >= n_records() )
            return ( false );

    TDbBookmark amark = bookmarks[idx];
    file->get_table().GotoL( amark );

    return ( true );
}

bool database_manipulator_c::save()
{
    if(!database || !file)
            return ( false );

    TBuf <64> ext( K_selections_extension );

    TBuf <256> filen;
    filen.Append( database->name() );
    filen.Append( _L("_") );
    filen.Append( file->name() );

    generic_cfg_c cfg( this );
    return cfg.save( database->path(), filen , ext );
}

bool database_manipulator_c::load()
{
    if(!database || !file)
            return ( false );

    TBuf <64> ext( K_selections_extension );

    TBuf <256> filen;
    filen.Append( database->name() );
    filen.Append( _L("_") );
    filen.Append( file->name() );

    generic_cfg_c cfg( this );
    return cfg.load( database->path(), filen , ext );
}
//    _LIT( K_selections_identifier,"[QuodDB SELECTION FILE]" );
void database_manipulator_c::ExternalizeL(RWriteStream& aStream) const
{
    TBufC <256> idt( K_selections_identifier );

    GC_WRITE_STR( idt );
    aStream.WriteInt32L( QUODDB_CURRENT_SEL_FILE_VERSION );

    aStream.WriteInt32L( use_heading ? 1 : 0 );
    aStream.WriteInt32L( heading_field_idx  );
    aStream.WriteInt32L( last_sort  );

    aStream.WriteInt32L( 0 ); // Reserved
    aStream.WriteInt32L( 0 ); // Reserved
    aStream.WriteInt32L( 0 ); // Reserved
    aStream.WriteInt32L( 0 ); // Reserved
    aStream.WriteInt32L( 0 ); // Reserved

    aStream.WriteInt32L( visible_columns.Count() );
    for( int n = 0; n < visible_columns.Count(); n++ )
            aStream.WriteInt32L( visible_columns[n]->id() );

    aStream.WriteInt32L( filters.Count() );
    for( int f = 0; f < filters.Count(); f++ )
    {
        db_filter_item_c *fifi = filters[f];

        aStream.WriteInt32L( (int) fifi->get_field()->id() );
        aStream.WriteInt32L( (int) fifi->get_type() );

        GC_WRITE_STR( fifi->get_data_item().get_as_str() );
    }
}

void database_manipulator_c::InternalizeL(RReadStream& aStream)
{
      TBuf <256> header;
      TBuf <256> stunt_des;

      int len = aStream.ReadInt32L();
      aStream.ReadL( header, len );

      if( header != K_selections_identifier  ) // Invalid file !
            return;

      int version = aStream.ReadInt32L();

      if( version > QUODDB_CURRENT_SEL_FILE_VERSION )
           return;

//     GC_READ_STR( stunt_des );
      use_heading = aStream.ReadInt32L() ? true : false;
      heading_field_idx = aStream.ReadInt32L();
      last_sort = aStream.ReadInt32L();

      aStream.ReadInt32L();//Reserved
      aStream.ReadInt32L();
      aStream.ReadInt32L();
      aStream.ReadInt32L();
      aStream.ReadInt32L();

      visible_columns.Reset();
      int numba = aStream.ReadInt32L();
      for( int n = 0; n < numba; n++ )
      {
          int id = aStream.ReadInt32L();
          dbm_field_c *ff = file->find_field( id );

          if( ff )
              visible_columns.Append( ff );
      }

    filters.Reset();
    int numbaf = aStream.ReadInt32L();

    for( int f = 0; f < numbaf; f++ )
    {
        int id = aStream.ReadInt32L();
        int type = aStream.ReadInt32L();

        GC_READ_STR( stunt_des );

        dbm_field_c *ff = file->find_field( id );

        if( ff )
        {
            db_data_item_c temp;
            temp.workout_from_str( ff , stunt_des );

            db_filter_item_c *fifi = new db_filter_item_c( file );
            fifi->start( ff, temp, (db_filter_item_type_e) type );

            filters.Append( fifi );
        }
    }

}

bool database_manipulator_c::sort_by( int sort_field_idx )
{
    last_sort = sort_field_idx;

    if( sort_field_idx == 0 )
    {
          file->get_table().SetNoIndex(); // Ordena por ordem de inserção, de futuro pode usar mais chaves ...
          return ( true );
    }
    else
    {
        int n_keys = 0;
        for( int n = 0; n < file->n_fields(); n++ )
        {
             if( file->get_field( n )->is_key() )
                    n_keys++;

             if( n_keys == sort_field_idx )
             {
                  file->get_table().SetIndex( file->get_field( n )->name() );
                  return ( true );
             }
        }

    }

    return ( false );
}

void database_manipulator_c::set_header_field( int heading_idx  )
{
    if( heading_idx != -1 )
    {
        heading_field_idx = heading_idx;
        use_heading = true;
    }
    else
    {
        use_heading = false;
    }

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef _NO_INTERFACE_PLEASE_

#include <aknselectionlist.h>
#include <aknquerydialog.h>

void database_manipulator_ui_c::add_line()
{
    if( !is_ok )
            return;

    data_view_form_c *form = data_view_form_c::NewL( this, file, DVFT_NEW_ITEM );
    form->PrepareLC();
    form->RunLD();
}

void database_manipulator_ui_c::delete_line( int idx )
{
    if( !is_ok )
            return;

    CDesCArrayFlat *delete_possibilities_array = CEikonEnv::Static()->ReadDesCArrayResourceL( R_DATA_VIEW_ARRAY_DELETE_POSSIBILITIES  );

    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_DELETE );

    TInt index( 0 );

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DATA_VIEW_LIST_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( delete_possibilities_array );
    dlg->SetHeaderTextL( *propmtText );

    TInt answer = dlg->RunLD();

    delete propmtText;

    if( answer )
    {
        if( index == 0 )
        {
            select_record( idx );
            file->get_table().DeleteL();
        }

        if( index == 1 ) //Delete ALL
        {
            for( int n = 0; n < bookmarks.Count(); n++ )
            {
                TDbBookmark amark = bookmarks[n];

                file->get_table().GotoL( amark );
                file->get_table().DeleteL();
            }
        }
    }

}

void database_manipulator_ui_c::edit_line( int idx )
{
    if( !is_ok )
            return;

    select_record( idx );

    // Update
    data_view_form_c *form = data_view_form_c::NewL( this, file , DVFT_EDIT_ITEM );
    form->PrepareLC();
    form->RunLD();
}

bool database_manipulator_ui_c::select_header()
{
    if( !is_ok )
            return false;

    CDesCArrayFlat *tArray = new (ELeave) CDesCArrayFlat(10);

    HBufC* strText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_NONE );
    tArray->AppendL( *strText );
    delete strText;

    for( int n = 0; n < file->n_fields(); n++ )
    {
        tArray->AppendL( file->get_field( n )->name() );
    }

    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_HEADER );
    CAknListQueryDialog* dlg;
    TInt index( 0 );
    dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DATA_VIEW_LIST_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( tArray );
    dlg->SetHeaderTextL( *propmtText );
    TInt answer = dlg->RunLD();

    delete propmtText;

    if( answer )
    {
        if( index == 0 )
        {
            use_heading = false;
            heading_field_idx = -1;
        }
        else
        {
            use_heading = true;
            heading_field_idx = index - 1;
        }

        return ( true );
    }
    return ( false );
}


bool database_manipulator_ui_c::select_sort()
{
    if( !is_ok )
            return ( false );

    if( !number_of_keys )
    {
        return ( false );
    }

    CDesCArrayFlat *tArray = new (ELeave) CDesCArrayFlat(10);

    HBufC* strText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_NONE );
    tArray->AppendL( *strText );
    delete strText;

    for( int n = 0; n < file->n_fields(); n++ )
    {
        if( file->get_field( n )->is_key() )
            tArray->AppendL( file->get_field( n )->name() );
    }

    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_SORT );

    TInt index( 0 );

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DATA_VIEW_LIST_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( tArray );
    dlg->SetHeaderTextL( *propmtText );
    TInt answer = dlg->RunLD();

    delete propmtText;

    if( answer )
    {
        sort_by( index );

        return ( true );
    }

    return ( false );
}

bool database_manipulator_ui_c::select_pages()
{
    if( !is_ok )
            return (false);

    CDesCArrayFlat *tArray = new (ELeave) CDesCArrayFlat(10);
    CArrayFixFlat <TInt> *indexArray = new(ELeave)CArrayFixFlat<TInt>( file->n_fields() );
    CArrayFixFlat <TInt> *visibleArray = new(ELeave)CArrayFixFlat<TInt>( file->n_fields() );

    for( int n = 0; n < file->n_fields(); n++ )
    {
            stunt.Zero();
            stunt.Append( _L("1\t") );
            stunt.Append( file->get_field( n )->name() );
            tArray->AppendL( stunt );

            if( is_visible( file->get_field( n ) ) )
            {
                visibleArray->AppendL( n );
            }
    }

    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_PAGES );

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( indexArray );

    dlg->PrepareLC( R_DATA_VIEW_LIST_QUERY_MULTISEL );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( tArray );
    dlg->SetHeaderTextL( *propmtText );

    CEikListBox *lb = dlg->ListBox();
    lb->SetSelectionIndexesL( visibleArray );

    TInt answer = dlg->RunLD();

    delete propmtText;

    bool ret = false;
    if( answer )
    {
        visible_columns.Reset();

        if( !indexArray->Count() )
        {
            visible_columns.Append( file->get_field( 0 ) );
        }

        for( int n = 0; n < indexArray->Count(); n++ )
        {
            visible_columns.Append( file->get_field( (*indexArray)[n] ) );
        }

        ret = true;
    }

    delete indexArray;
    delete visibleArray;

    return ( ret );
}

bool database_manipulator_ui_c::select_filters_add()
{
    if( !is_ok )
            return (false);

    if( !showing )
            return (false);

    CDesCArrayFlat *operations_array = CEikonEnv::Static()->ReadDesCArrayResourceL( showing->type() == DFT_TEXT ? R_DATA_VIEW_ARRAY_OPERATION_EXTRA : R_DATA_VIEW_ARRAY_OPERATION_SIMPLE );
    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_SELECT_OPERATION );

    TInt index( 0 );

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DATA_VIEW_LIST_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( operations_array );
    dlg->SetHeaderTextL( *propmtText );
    TInt answer = dlg->RunLD();

    delete propmtText;

    if( answer )
    {
        gn_field_query_c qr;
        bool ret = qr.query_field(  showing , showing->name() );

        if( ret )
        {
            db_filter_item_c *fil = new db_filter_item_c( file );

            if( fil )
            {
                fil->start( showing, qr.get_data_item() , (db_filter_item_type_e) index );
                filters.Append( fil );

                return ( true );
            }
        }

    }

    return ( false );
}

bool database_manipulator_ui_c::select_filters_remove()
{
    if( !is_ok )
            return (false);

    if( !filters.Count() )
                return (false);

    CDesCArrayFlat *operations_array = CEikonEnv::Static()->ReadDesCArrayResourceL( R_DATA_VIEW_ARRAY_OPERATION_EXTRA  );
    CDesCArrayFlat *tArray = new (ELeave) CDesCArrayFlat(10);

    TBuf <512> stunt;
    for( int n = 0; n < filters.Count(); n++ )
    {
        stunt.Zero();
        stunt.Append( filters[n]->get_field()->name() );
        stunt.Append( _L(" ") );
        stunt.Append( (*operations_array)[(int)filters[n]->get_type()] );
        stunt.Append( _L(" ") );
        stunt.Append( filters[n]->get_data_item().get_as_str() );

        tArray->AppendL( stunt );
    }

    delete operations_array;

    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_SELECT_FILTER_TO_DEL );

    TInt index( 0 );

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DATA_VIEW_LIST_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( tArray );
    dlg->SetHeaderTextL( *propmtText );
    TInt answer = dlg->RunLD();

    delete propmtText;


    if( answer )
    {
        delete filters[index];

        filters.Remove( index );

        return ( true );
    }

    return ( false );
}

int database_manipulator_ui_c::select_lookup()
{
    if( !is_ok )
            return (false);

    if( !showing )
            return (false);

    CDesCArrayFlat *operations_array = CEikonEnv::Static()->ReadDesCArrayResourceL( showing->type() == DFT_TEXT ? R_DATA_VIEW_ARRAY_OPERATION_EXTRA : R_DATA_VIEW_ARRAY_OPERATION_SIMPLE );
    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_SELECT_OPERATION );

    TInt index( 0 );

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DATA_VIEW_LIST_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( operations_array );
    dlg->SetHeaderTextL( *propmtText );
    TInt answer = dlg->RunLD();

    delete propmtText;

    if( answer )
    {
        gn_field_query_c qr;
        bool ret = qr.query_field(  showing , showing->name() );

        if( ret )
        {
            db_filter_item_c *fil = new db_filter_item_c( file );

            if( fil )
            {
                int ret = -1;
                fil->start( showing, qr.get_data_item() , (db_filter_item_type_e) index );

                for( int n = 0; n < bookmarks.Count(); n++ )
                {
                    TDbBookmark amark = bookmarks[n];

                    file->get_table().GotoL( amark );
                    file->get_table().GetL();

                    if( fil->check_on_currrent_record() )
                    {
                        ret = n;
                        break;
                    }

                }

                delete fil;

                return ( ret );
            }


        }

    }

    return ( -1 );
}

#endif
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

