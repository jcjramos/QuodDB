/*-------------------------------------------------------------------------------


            MOBICRM - DB master class
            Handles Database related issues


- dbm_master_c Handles a database.
Each Database is stored in a multi-table file DBNAME.DB & a description
file called DBNAME.CDB

- dbm_file_c handles a table in the database

- dbm_field handles a column in a table

JCR 3.2004
------------------------------------------------------------------------------*/
#define _DBMASTER_EXTENSIONS_
#include "dbmaster.h"

#include <d32dbms.h>
#include <eikenv.h>



// ATT: Dir C:\Symbian\Series60_1_2_B\epoc32\winsb\c\System\apps\APPNAME in the emulator

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

dbm_field_c::dbm_field_c( int id , TBufC <FIELD_SIZE> &name , dbm_field_type_e type  , int __max_len  ):_name( name )
{
    _extra = 0;
    _is_hidden = 0;

    _id = id;
    _type = type;
    _max_len = __max_len;

    _is_key = DFK_NO;

    pfield_text = 0;
}

dbm_field_c::~dbm_field_c()
{
    delete pfield_text;
}

void dbm_field_c::add_extra_desc( const TDesC &new_text )
{
    if( !pfield_text )
    {
        pfield_text = new CDesCArrayFlat( 10 );
    }

    pfield_text->AppendL( new_text );
}

TDbColType dbm_field_c::db_type() // Translates internal to DB types ...
{
    switch( type() )
    {
        case DFT_INT:return EDbColInt32;
        case DFT_TEXT:return EDbColText;
        case DFT_DATE:return EDbColDateTime;
        case DFT_TIME:return EDbColDateTime;
        case DFT_FLOAT:return EDbColReal64;
        case DFT_LIST:return EDbColInt32; //ATT, KEEPS IDXs !!!
        case DFT_BIGNUMBER:return EDbColInt64;
        case DFT_YES_NO:return EDbColBit;
        case DFT_PHONE_NUMBER:return EDbColText;
        case DFT_DATE_TIME:return EDbColDateTime;
        case DFT_TEXT_NUMBER:return EDbColText;

        default:
            CEikonEnv::Static()->InfoWinL( _L("Error"),_L("dbm_field_c::db_type() Invalid type") );
            return EDbColInt32;
    };

}

void dbm_field_c::link_to( int file_id, int field_id, int file_id_field_to_show )
{
    link_details.file_id = file_id;
    link_details.field_id = field_id;
    link_details.field_id_to_show = file_id_field_to_show;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
dbm_file_c::dbm_file_c( int id , TBufC <FIELD_SIZE> name  ):_name( name )
{
    daddy = 0;
    _id = id;

    is_open = false;
}

dbm_file_c::~dbm_file_c()
{
    if( is_open )
        close();

    for( int n = 0; n < n_fields(); n++ )
    {
        delete get_field( n );
    }

    fields.Close();
    to_close.Close();
}
dbm_field_c *dbm_file_c::insert_field_kex( int id ,TBufC <FIELD_SIZE> name , dbm_field_type_e _type , int max_len, dbm_field_key_type_e ktype )
{
    dbm_field_c *ff = insert_field_ex( id , name , _type, max_len  );
    ff->set_key_type( ktype  );
    return ff;
}

dbm_field_c *dbm_file_c::insert_field_ex( int id , TBufC <FIELD_SIZE> name , dbm_field_type_e _type, int __max_len  )
{
    fields.Append( new dbm_field_c( id , name , _type, __max_len ) );

    return fields[ n_fields()-1 ];
}

dbm_field_c *dbm_file_c::find_field( int id )
{
    for( int n = 0; n < n_fields(); n++ )
    {
        if( get_field(n)->id() == id )
                return  get_field(n);
    }

    return ( 0 );
}

dbm_field_c *dbm_file_c::find_field( TPtrC name )
{
    for( int n = 0; n < n_fields(); n++ )
    {
        if( get_field(n)->name() == name )
                return  get_field(n);
    }

    return ( 0 );

}

int dbm_file_c::insert_field( dbm_field_c *afield )
{
    fields.Append( afield );

    return n_fields();
}

bool dbm_file_c::init( dbm_master_c *_daddy )
{
    daddy = _daddy;

    return ( true );
}

bool dbm_file_c::has_data()
{
    bool has_data = false;

    CDbTableNames *tnames = daddy->Database().TableNamesL();

    if( tnames )
    {
        for( int n =0; n < tnames->Count(); n++ )
        {
            if( (*tnames)[n] == name() )
            {
                has_data = true;
                break;
            }
        }

        delete tnames;
    }

    return has_data;
}

bool dbm_file_c::verify_tables()
{
    bool must_create_this_table = has_data() ? false : true;

    int number_of_keys = 0;
    for( int n = 0; n < fields.Count(); n++ )
    {
        if( fields[n]->is_key() ) // Used beloow
                 number_of_keys++;
    }

    if( must_create_this_table )
    {

        // Create Tables
        CDbColSet* columns=CDbColSet::NewLC();

        if( !fields.Count() )
                return ( false );

        for( int n = 0; n < fields.Count(); n++ )
        {

            if( fields[n]->db_type() == EDbColText )
                    columns->AddL( TDbCol( fields[n]->name(), fields[n]->db_type() , fields[n]->max_len() ) );
                else
                    columns->AddL( TDbCol( fields[n]->name(), fields[n]->db_type() ) );
        }

        int ret = daddy->Database().CreateTable( name(),*columns);
        if( ret )
            dbm_master_error( _L("Database().CreateTable"), ret );

        User::LeaveIfError( ret );

        CleanupStack::PopAndDestroy();

        if( number_of_keys )
        {
            for( int n = 0; n < fields.Count(); n++ )
            {
                if( !fields[n]->is_key() )
                        continue;

                // Create Indexs
                CDbKey* key= CDbKey::NewLC();

                if( fields[n]->get_key_type() == DFT_UNIQUE )
                    key->MakeUnique();

                // add the key columns
                TDbKeyCol idd( fields[n]->name() );
                key->AddL( idd );
                        // create the index
                User::LeaveIfError( daddy->Database().CreateIndex(fields[n]->name(),name(),*key));

                CleanupStack::PopAndDestroy();
            }
        }

    }
    else
    {// OK The Table exists ... but lezt see if all the fields match
        CDbColSet*colset =  daddy->Database().ColSetL( name() );
        bool the_table_was_changed = false;

        if( colset ) // MATCHES
        {
            // CHECK FOR TYPE CHANGES !!!
            bool types_changed = false;
            for( int t = 0; t < n_fields(); t++ )
            {
                const TDbCol *col = colset->Col( get_field(t)->name() );
                if( col && col->iType != get_field(t)->db_type() )
                {
                    colset->Remove( col->iName );

                    types_changed = true;
                }
            }

            if( types_changed ) // Will create the droped columns again with the correct types !
            {
                daddy->Database().AlterTable( name(),*colset);
                delete colset;

                colset =  daddy->Database().ColSetL( name() );
            }

            for( int n = 0; n < n_fields(); n++ )
            {
                const TDbCol *col = colset->Col( get_field(n)->name() );

                if( !col ) // Must add the column !
                {
                    if( fields[n]->db_type() == EDbColText )
                            colset->AddL( TDbCol( fields[n]->name(), fields[n]->db_type() , fields[n]->max_len() ) );
                        else
                            colset->AddL( TDbCol( fields[n]->name(), fields[n]->db_type() ) );

                    the_table_was_changed = true;
                }
            }

            if( colset->Count() > n_fields() ) // Tem colunas por usar ?
            {
                for( int n = 0; n < colset->Count(); n++ )
                {
                    const TDbCol &col = (*colset) [n+1];

                    if( !find_field( col.iName ) )
                    {
                        colset->Remove( col.iName );
                        n--;

                        the_table_was_changed = true;
                    }
                }

            }

            if( the_table_was_changed )
                daddy->Database().AlterTable( name(),*colset);

            delete colset;

            CDbIndexNames *idx_names =  daddy->Database().IndexNamesL( name() );

            if( number_of_keys )   // Update the indexes if necessary !
            {
                for( int n = 0; n < fields.Count(); n++ )
                {
                    if( !fields[n]->is_key() )
                            continue;

                    bool exists = false;
                    for( int i = 0; i < idx_names->Count(); i++ )
                    {
                        if( fields[n]->name() == (*idx_names)[i] )
                        {
                            exists = true;
                            break;
                        }
                    }

                    if ( exists )
                        continue;

                    // Create Indexs
                    CDbKey* key= CDbKey::NewLC();

                    if( fields[n]->get_key_type() == DFT_UNIQUE )
                        key->MakeUnique();

                    // add the key columns
                    TDbKeyCol idd( fields[n]->name() );
                    key->AddL( idd );
                            // create the index
                    User::LeaveIfError( daddy->Database().CreateIndex(fields[n]->name(),name(),*key));

                    CleanupStack::PopAndDestroy();
                }
            }

            delete idx_names;

        }
    }

//    CEikonEnv::Static()->InfoWinL( _L("Error"),_L("Verify tables DONE") );

    return ( true );
}

bool dbm_file_c::open( bool read_only )
{
    if( !read_only )
    {
        verify_tables();
    }
    else
    {
        if( !has_data() ) // AInda não existe ? tem de criar primeiro !!!!
        {
            verify_tables();
        }
    }

    TInt err = table.Open( daddy->Database(), name() , read_only ? RDbTable::EReadOnly : RDbTable::EUpdatable);
    User::LeaveIfError(err);

    colSet = table.ColSetL();

    if( !colSet )
        return ( false );

    for( int n = 0; n < n_fields(); n++ )
    {
        dbm_field_c *f = get_field( n );

        f->col_idx_in_table = colSet->ColNo( f->name() );

        if( f->is_link() )
        {
            handle_link_field( f );
        }
    }

    is_open = true;

    return true;
}

void dbm_file_c::close()
{
     delete colSet;

     table.Close();

     is_open = false;

     for( int n = 0; n < to_close.Count(); n++ )
     {
        if( to_close[n]->get_is_open() )
            to_close[n]->close();

     }

     to_close.Reset(); // USed to store linked files
}

bool dbm_file_c::handle_link_field( dbm_field_c *f )
{
    if( !f )
        return ( false );

    link_details_s *pdet = f->get_link_details();

    pdet->file = daddy->find_file( pdet->file_id );

    if( !pdet->file )
    {
        CEikonEnv::Static()->InfoWinL( _L("Error"),_L("handle_link_field failed - invalid file") );
        return ( false );
    }

    pdet->field = pdet->file->find_field( pdet->field_id );
    pdet->field_to_show = pdet->file->find_field( pdet->field_id_to_show );

    if( !pdet->field )
    {
        CEikonEnv::Static()->InfoWinL( _L("Error"),_L("handle_link_field failed - invalid field") );
        return ( false );
    }

    if( !pdet->field_to_show )
        pdet->field_to_show = pdet->field;

    if( !pdet->file->get_is_open() )
    {
        pdet->file->open( true );

        to_close.Append( pdet->file );
    }

    return ( true );
}

bool dbm_file_c::delete_field( int idx )
{
    if( idx >= 0 && idx < n_fields() )
    {
         delete get_field( idx );
         fields.Remove( idx );

         return ( true );
    }

    return ( false );
}

bool dbm_file_c::valid_field_name( TPtrC fname )
{
    if( !dbm_master_valid_name( fname ) )
            return ( false );

     TBuf <256> _name( fname );
     _name.UpperCase();

     TBuf <256> stunt( fname );

     // EXISTS ?
     for( int n = 0; n < n_fields(); n++ )
     {
        stunt.Zero();
        stunt.Append( get_field( n )->name() );
        stunt.UpperCase();

        if( stunt == _name )
                return ( false );
     }

     if( has_data() )
     {
         // Might exist in a low level table !!! MANIACCCC
        CDbColSet*colset =  daddy->Database().ColSetL( name() );

        if( colset )
        {
            const TDbCol *col = colset->Col( fname );
            if( col )
            {
                delete colset;
                return ( false );
            }

            delete colset;
        }

     }

     return ( true );
}

int dbm_file_c::get_next_empty_field_id()
{
    int last_id = 0;

    for( int n = 0; n < n_fields(); n++ )
    {
        if( get_field( n )->id() > last_id )
            last_id = get_field( n )->id();
    }

    return (last_id + 1 );
}

bool dbm_file_c::sync_links() //HERE
{
    for( int n = 0; n < n_fields(); n++ )
    {
        dbm_field_c *f = get_field( n );

        if( f->is_link() )
        {
            link_details_s *pdet = f->get_link_details();

            if( f->get_col_idx_in_table() == KDbNullColNo || get_table().IsColNull(f->get_col_idx_in_table()) )
            {
                pdet->status = false;
            }
            else
            {
                pdet->file->get_table().SetIndex( pdet->field->name() );

                TDbSeekKey kk;

                switch( f->db_type() )
                {
                    case EDbColInt32:kk.Add( get_table().ColInt(f->get_col_idx_in_table()) );break;
                    case EDbColText:kk.Add( get_table().ColDes(f->get_col_idx_in_table()) );break;
                    case EDbColDateTime:kk.Add( get_table().ColTime(f->get_col_idx_in_table()) );break;
                    case EDbColReal64:kk.Add( get_table().ColReal(f->get_col_idx_in_table()) );break;
                    case EDbColInt64:kk.Add( get_table().ColInt64(f->get_col_idx_in_table()) );break;
                    case EDbColBit:kk.Add( get_table().ColUint8(f->get_col_idx_in_table()) );break;
                }

                pdet->status = pdet->file->get_table().SeekL( kk );

                if( pdet->status )
                {
                    pdet->file->get_table().GetL();
                }
            }

        }
    }

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
dbm_master_c::dbm_master_c(  TBufC <FNAME_SIZE> &__path , TBufC <FNAME_SIZE> & __name ):_path( __path),_name(__name)
{
    store = 0;
    is_init = false;
}

dbm_master_c::~dbm_master_c()
{
    if( !is_init )
            return;

    save_defs();

    for( int n = 0; n < n_files(); n++ )
    {
        delete get_file( n );
    }

    files.Close();

    if( store )
    {
		database.Compact(); //New in 2006 version

    	database.Close();

        delete store;
    }

    fsSession.Close(); //close the file session
}

bool dbm_master_c::save_defs()
{

    TBufC <255> ext( K_cfg_extension );

    generic_cfg_c cfg( this );
    return ( cfg.save( path(), name(), ext ) );
}

bool dbm_master_c::init_files()
{
    TBufC <255> ext( K_cfg_extension );

    generic_cfg_c cfg( this );
    return ( cfg.load( path(), name(), ext  ) );
}

bool dbm_master_c::init_tables()
{
    for( int n = 0; n < n_files(); n++ )
    {
        get_file( n )->init( this );
    }

    return ( true );
}

bool dbm_master_c::init()
{
    is_init = true;

    init_files();

	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return false;
    }

//*#error PASSASE ALGO COM A PATH ... não chega em condicoes !!!!!
//    _LIT( pata,"C:\\system\\apps\\QUODDB\\App.db");

    TBuf <256> pata( path() );
    pata.Append( name() );
    pata.Append( K_db_extension );

    TTime dummy;
    if( fsSession.Modified( pata, dummy ) != KErrNone ) // File does not exist !
    {
       //CEikonEnv::Static()->InfoWinL( _L("Error"),pata );
       
        store = CPermanentFileStore::ReplaceL( fsSession, pata,EFileRead|EFileWrite);

        if( !store )
            return ( false );

    	store->SetTypeL(store->Layout());

    	TStreamId id=database.CreateL(store);
				// Keep database id as root of store
        store->SetRootL(id);

        store->CommitL();

        // Empty now, to create a .CDB file with the .DB !
        save_defs();
    }
    else
    {
        store = CPermanentFileStore::OpenL( fsSession,pata,EFileRead|EFileWrite);

        if( !store )
            return ( false );

        database.OpenL( store, store->Root() );
    }

   init_tables();

    return ( true );
}

dbm_file_c *dbm_master_c::get_file( int idx )
{
    return files[ idx ];
}

int dbm_master_c::n_files()
{
    return files.Count();
}

dbm_file_c *dbm_master_c::find_file( int id )
{
    for( int n = 0; n < n_files(); n++ )
    {
        if( get_file( n )->id() == id )
            return get_file( n );
    }

    return ( 0 );
}

dbm_file_c *dbm_master_c::find_file( TPtrC name )
{
    for( int n = 0; n < n_files(); n++ )
    {
        if( get_file( n )->name() == name )
            return get_file( n );
    }

    return ( 0 );

}

int dbm_master_c::insert_file( dbm_file_c *file )
{
//    file->init( this );

    files.Append( file );

    return n_files();
}


void dbm_master_c::ExternalizeL(RWriteStream& aStream) const
{
    TBufC <255> idt( K_cfg_identifier );

    GC_WRITE_STR( idt ); ;

    aStream.WriteInt32L( DM_MASTER_CURRENT_CFG_VERSION );;

    aStream.WriteInt32L( files.Count() );

    for( int n = 0; n < files.Count(); n++ )
    {
        dbm_file_c *file = files[n];

        aStream.WriteInt32L( file->id() );
        //aStream.WriteL( file->name() , FIELD_SIZE );
        GC_WRITE_STR( file->name() );

        aStream.WriteInt32L( file->n_fields() );

        for( int f = 0; f < file->n_fields(); f++ )
        {
                 dbm_field_c *field = file->get_field( f );

                aStream.WriteInt32L( field->id() );
                //aStream.WriteL( field->name(), FIELD_SIZE );
                GC_WRITE_STR( field->name() );

                aStream.WriteInt32L( field->type() );
                aStream.WriteInt32L( field->max_len() );
                aStream.WriteInt32L( field->extra_number() );
                aStream.WriteInt32L( (int) field->get_key_type() );
                aStream.WriteInt32L( field->is_hidden() );

                aStream.WriteInt32L( field->get_link_details()->file_id );
                aStream.WriteInt32L( field->get_link_details()->field_id );
                aStream.WriteInt32L( field->get_link_details()->field_id_to_show );

                aStream.WriteInt32L( field->get_n_extra_descs() );

                for( int s = 0; s < field->get_n_extra_descs(); s++ )
                {
                        GC_WRITE_STR( field->get_extra_desc( s ) );
                }

        }
    }
}

void dbm_master_c::InternalizeL(RReadStream& aStream)
{
      TBuf <FIELD_SIZE> header;
      TBuf <FIELD_SIZE> stunt_des;

      int len = aStream.ReadInt32L();
      aStream.ReadL( header, len );

      if( header != K_cfg_identifier ) // Invalid file !
            return;

      int version = aStream.ReadInt32L();

      if( version > DM_MASTER_CURRENT_CFG_VERSION )
           return;

      int number_of_files = aStream.ReadInt32L();

      for( int n = 0; n < number_of_files; n++ )
      {
            int id = aStream.ReadInt32L();

            //aStream.ReadL( stunt_des );
            GC_READ_STR( stunt_des );

            dbm_file_c *file = new dbm_file_c( id, stunt_des );

            if( !file )
                continue;

            int number_of_fields = aStream.ReadInt32L();

            for( int f = 0; f < number_of_fields; f++ )
            {
                int id = aStream.ReadInt32L();
                //aStream.ReadL( stunt_des );
                GC_READ_STR( stunt_des );

                int type = aStream.ReadInt32L();
                int len = aStream.ReadInt32L();
                int extra = aStream.ReadInt32L();
                int is_key = aStream.ReadInt32L();
                int is_hidden = 0;
                int lfile_id=0,lfield_id=0,lfield_show = 0;

                if( version > 1 )
                {
                        is_hidden = aStream.ReadInt32L();
                        if( version > 2 )
                        {
                            lfile_id = aStream.ReadInt32L();
                            lfield_id = aStream.ReadInt32L();
                            lfield_show = aStream.ReadInt32L();
                        }
                }

                dbm_field_c *field = file->insert_field_ex( id, stunt_des, (dbm_field_type_e) type, len );

                field->set_extra_number( extra );
                field->set_key_type( (dbm_field_key_type_e) is_key );
                field->set_is_hidden( is_hidden );

                if( lfile_id )
                {
                    field->link_to( lfile_id, lfield_id, lfield_show );
                }

                int n_extra_field_text = aStream.ReadInt32L();
                for( int s = 0; s < n_extra_field_text; s++ )
                {
                    GC_READ_STR( stunt_des );
                    field->add_extra_desc( stunt_des );
                }

            }

            insert_file( file );
      }
}

bool dbm_master_c::delete_all()
{
    RFs privateSession;
  	TInt fsret = privateSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return false;
    }

    TBuf <FNAME_SIZE> databasen( path() );
    TBuf <FNAME_SIZE> confign( path() );

    databasen.Append( name() );
    databasen.Append( K_db_extension );

    confign.Append( name() );
    confign.Append( K_cfg_extension );

    int ret1 = privateSession.Delete( databasen );
    int ret2 = privateSession.Delete( confign );

    privateSession.Close();

    return ( ret1 == KErrNone && ret2 == KErrNone ? true : false );
}

bool dbm_master_c::delete_file( int idx )
{
    if( idx >= 0 && idx < n_files() )
    {
         dbm_file_c *file = get_file( idx );

         if( file->has_data() )
         {
            Database().DropTable( file->name() );
         }

         delete file;
         files.Remove( idx );

         return ( true );
    }

    return ( false );
}

bool dbm_master_c::valid_file_name( TPtrC name )
{
    if( !dbm_master_valid_name( name ) )
            return ( false );

     TBuf <256> _name( name );
     _name.UpperCase();

     TBuf <256> stunt( name );

     // EXISTS ?
     for( int n = 0; n < n_files(); n++ )
     {
        stunt.Zero();
        stunt.Append( get_file( n )->name() );
        stunt.UpperCase();

        if( stunt == _name )
                return ( false );
     }

     return ( true );
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
Rules for DBMS names
Tables, indexes and columns are named objects within a database. A DBMS name must begin with an alphabetic character, after which any alphabetic, numeric or the _ (underscore) character may be used. DBMS names are also limited to 64 characters in length.

Table names must be unique within a database, and columns and indexes must have unique names within the table to which they belong. For the purposes of uniqueness and identification, the names are folded before comparison, so two columns named column_one and Column_ONE are considered to have the same name.

Names in a database have no further restrictions.
*/
bool dbm_master_valid_name( TPtrC name )
{
    if( !name.Length() || name.Length() > 63 )
            return false;
    // ALPHA ou _
    TChar first( name[0] );
    TChar unders( '_' );

    if( !first.IsAlpha() )
            return false;

    for( int n = 1; n < name.Length(); n++ )
    {
        TChar ch( name[n] );

        if( !ch.IsAlphaDigit() && ch != unders )
            return ( false );
    }

    return ( true );
}

// CHanges tabs to spaces to allow them to be correctly displayed.
bool dbm_master_clean_tabs( TDes &name )
{
    TChar space( ' ' );
    TChar tab( '\t' );

    for( int n=0; n < name.Length(); n++ )
    {
        if( (unsigned)name[n] == (unsigned)tab )
        {
            name[n] = space;
        }
    }

    return ( true );
}

void dbm_master_error( TPtrC desc, int id )
{
    TBuf <1024> msg( desc );
    msg.Append( _L("  - Id= ") );
    msg.AppendNum( id );

    CEikonEnv::Static()->InfoWinL( _L("Error"),msg );
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/*
_LIT( K_WorkingPath,"C:\\system\\apps\\QUODDB\\");
_LIT( K_db_name,"TESTE");


void teste()
{
    dbm_master_c obj( K_WorkingPath, K_db_name );
    obj.init();
}
*/
