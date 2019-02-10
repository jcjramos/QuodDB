/*------------------------------------------------------------------------------

                    Class DB Import / Export


    JCR 3.2003
------------------------------------------------------------------------------*/
#include "dbImportExport.h"
#include "dbDataItem.h"
#include "csvReader.h"

#include <AknWaitDialog.h>
#include <aknprogressdialog.h> 
#include <eikenv.h>
#include <eikprogi.h>

#include "genAppRes.h"

#ifndef __GEN_APP_RESOURCE_INCLUDE_FILE__
#error  Please create a GenAppRes.H file including the AppName.RSH & RSH files
#error  Then include dataView.RSS in your application resource file!
#endif
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <f32file.h>
#include <aknlistquerydialog.h>



class file_query_dialog_c
{
        TInt index;

        CDesCArrayFlat *folders;
        CDesCArrayFlat *drive_letters;

        RFs fsSession;

        RArray <int> *folder_or_file;

        bool build_folder_array( TPtrC pata , bool valid_going_back = true );
        bool build_drive_list();

        bool show_list_query();

    public:
        file_query_dialog_c();
        ~file_query_dialog_c();

        bool do_it( TDes &aFileName );
        bool do_it_for_save( TDes &aFileName );
};

file_query_dialog_c::file_query_dialog_c()
{
    folders = new (ELeave) CDesCArrayFlat( 10 );
    drive_letters = new (ELeave) CDesCArrayFlat( 10 );

    folder_or_file = new (ELeave) RArray <int>;

    fsSession.Connect(); // start a file session
}

file_query_dialog_c::~file_query_dialog_c()
{
     fsSession.Close(); //close the file session

     folder_or_file->Close();

    delete folders;
    delete drive_letters;
    delete folder_or_file;
}

bool file_query_dialog_c::do_it(TDes &aFileName)
{
    build_drive_list();

    if( folders->Count() > 1 )
    {
        if( !show_list_query() )
            return ( false );
    }
    else
    {
        index = 0;
    }

    TBuf <256> path( (*drive_letters)[index] );

    build_folder_array( path , false );

    while( show_list_query() )
    {

        if( (*folder_or_file)[index] == 0 ) // File
        {
            path.Append( (*folders)[index] );
            aFileName.Copy( path );
            return ( true );
        }

        if( (*folder_or_file)[index] == 1 ) // Folder
        {
            path.Append( (*folders)[index] );
            path.Append( TChar( '\\') );
        }

        if( (*folder_or_file)[index] == 2 ) // BACK !!!
        {
            TInt pos = path.LocateReverse( TChar('\\') );
            if( pos != KErrNotFound )
                        path.SetLength( pos );

            pos = path.LocateReverse( TChar('\\') );
            if( pos != KErrNotFound )
                        path.SetLength( pos );

        }

        build_folder_array( path , true );
    }

    return ( false );
}

bool file_query_dialog_c::show_list_query()
{
    CDesCArrayFlat *operations_array = new CDesCArrayFlat( 10 );

    for( int n = 0; n < folders->Count(); n++ )
    {
#ifdef __OS91__
		if( (*folders)[n].Find(_L("sys") ) >= 0 ) 
				continue;
#endif
        operations_array->AppendL( (*folders)[n] );
    }

//    HBufC * propmtText = CEikonEnv::Static()->AllocReadResourceL( R_DB_IMPORT_EXPORT_STR_SELECT_FILE );

    index = 0;

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DB_IMPORT_EXPORT_FILE_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( operations_array );
//    dlg->SetHeaderTextL( *propmtText );
    TInt answer = dlg->RunLD();

 //   delete propmtText;

    if( answer )
    {
        return ( true );
    }

    return ( false );
}

bool file_query_dialog_c::build_drive_list()
{

    folders->AppendL( _L("Phone Memory") );
    drive_letters->AppendL( _L("C:\\") );

    if( fsSession.IsValidDrive(EDriveE) )
    {
        folders->AppendL( _L("Memory Card") );
        drive_letters->AppendL( _L("E:\\") );
    }

    /*
    TInt driveNumber=EDriveA;
	for (driveNumber=EDriveA; driveNumber<=EDriveZ;driveNumber++)
    {
        if (fsSession.IsValidDrive(driveNumber))
        {
            TBuf <256> desc;
            TBuf <256> ddesc;

            fsSession.GetDriveName( driveNumber, desc );

            TChar ch;
            fsSession.DriveToChar(driveNumber,ch);
            ddesc.Append( ch );
            ddesc.Append( TChar(':') );
            ddesc.Append( TChar('\\') );

            if( desc.Length() )
            {
                folders->AppendL( ddesc );
                drive_letters->AppendL( ddesc );
            }
        }
    }
    */

    return ( true );
}

bool file_query_dialog_c::build_folder_array( TPtrC _pata,  bool valid_going_back  )
{

    TBuf <256> pata( _pata );
    pata.Append( _L("*") );


    CDir* dirList;

    fsSession.GetDir( pata , /*KEntryAttMaskSupported*/ KEntryAttMatchMask, ESortByName, dirList );

    delete folders;
    folders = new (ELeave) CDesCArrayFlat( 10 );

    folder_or_file->Reset();

    if( valid_going_back )
    {
        folders->AppendL( _L("..") );
        folder_or_file->Append( 2 );
    }

    if( dirList && dirList->Count() )
    {

        for (TInt i=0;i<dirList->Count();i++)
        {
            TBuf <256> ooname( (*dirList)[i].iName );


            if( (*dirList)[i].IsDir() )
            {
                    folders->AppendL( ooname );
                    folder_or_file->Append( 1 );
            }
            else
            {
                ooname.UpperCase();

                _LIT( k_dot_csv, ".CSV" );
                _LIT( k_dot_txt, ".TXT" );

                if( ooname.Find( k_dot_csv ) != KErrNotFound )
                {
                    folders->AppendL( ooname );
                    folder_or_file->Append( 0 );
                }

                if( ooname.Find( k_dot_txt ) != KErrNotFound )
                {
                    folders->AppendL( ooname );
                    folder_or_file->Append( 0 );
                }

            }

        }
    }

    delete dirList;

    return ( true );
}

bool file_query_dialog_c::do_it_for_save( TDes &aFileName )
{
    build_drive_list();

    if( folders->Count() > 1 )
    {
        if( !show_list_query() )
            return ( false );
    }
    else
    {
        index = 0;
    }

  HBufC* strText = CEikonEnv::Static()->AllocReadResourceL( R_DB_IMPORT_EXPORT_STR_FILE_NAME );
  TBuf <1024> text( *strText );

  TBuf <256> ffname( aFileName );
  CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog( ffname, CAknQueryDialog::ENoTone );
  if (dlg->ExecuteLD( R_DB_IMPORT_EXPORT_QUERY_TEXT_DIALOG , text ))
  {
        aFileName.Copy( (*drive_letters)[index] );
        aFileName.Append( ffname );
        return ( true );
  }

  return ( false );
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
db_import_export_c::db_import_export_c( dbm_file_c *_file )
{
    file = _file;

    strTextError = CEikonEnv::Static()->AllocReadResourceL( R_DB_IMPORT_EXPORT_STR_ERROR );
    strTextInvalidFormat = CEikonEnv::Static()->AllocReadResourceL( R_DB_IMPORT_EXPORT_STR_INVALID_FORMAT );

    current_separator = 0;
    separators.Append( TChar(',') );
    separators.Append( TChar(';') );
    separators.Append( TChar('\t') );
    separators.Append( TChar(' ') );
}

db_import_export_c::~db_import_export_c()
{
    delete strTextError;
    delete strTextInvalidFormat;

    separators.Close();
    undo_bookmarks.Close();
}

bool db_import_export_c::write_eol_to_rfile( RFile &rfile )
{
    TBuf8 <256> buff;
    buff.Append( TChar('\r') );
    buff.Append( TChar('\n') );

    rfile.Write( buff );

    return ( true );
}

bool db_import_export_c::write_separator_to_rfile( RFile &rfile )
{
    TBuf8 <256> buff;
    buff.Append( separators[ current_separator ] );
    rfile.Write( buff );

    return ( true );
}

bool db_import_export_c::write_str_to_rfile( RFile &rfile, TPtrC str, bool with_commas  )
{
    TBuf8 <256> buff;

    if( with_commas )
    {
        buff.Zero();
        buff.Append( TChar('"') );
        rfile.Write( buff );
    }

    buff.Copy( str ); // Handles the convertion between 16 & 8 bit strings..
    rfile.Write( buff );

    if( with_commas )
    {
        buff.Zero();
        buff.Append( TChar('"') );
        rfile.Write( buff );
    }

    return ( true );
}

bool db_import_export_c::do_export_csv( TPtrC file_path )
{
    RFs fsSession;
    TParse	filestorename;
    RFile rfile;

	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return false;
    }

    // Open the direct file store
	fsSession.Parse( file_path , filestorename );

    if( rfile.Create( fsSession, filestorename.FullName(),EFileWrite | EFileStreamText ) != KErrNone )
    {
            fsSession.Close(); //close the file session
            return ( false );
    }


    for( int n = 0; n < file->n_fields(); n++ )
    {
        write_str_to_rfile( rfile, file->get_field(n)->name() );

        if( n + 1 < file->n_fields() )
                    write_separator_to_rfile( rfile );
    }

    write_eol_to_rfile( rfile );

    for (file->get_table().FirstL(); file->get_table().AtRow(); file->get_table().NextL())
    {
          file->get_table().GetL();

          for( int n = 0; n < file->n_fields(); n++ )
          {
              dbm_field_c *ff = file->get_field(n);

              db_data_item_c it;
              it.build_from_db( ff , file );

              bool comatize = ( ff->visual_type()== DFT_TEXT ? true : false );
              write_str_to_rfile( rfile, it.get_as_str(), comatize );

              if( n + 1 < file->n_fields() )
                   write_separator_to_rfile( rfile );

          }

          write_eol_to_rfile( rfile );
    }

    rfile.Close();
    fsSession.Close(); //close the file session

    return ( true );

}

bool db_import_export_c::do_import_structure( TPtrC path )
{
    csv_reader_c csv( separators[ current_separator ] );

    if( !csv.start( path ) )
    {
        CEikonEnv::Static()->InfoWinL( _L("Error, cannot read"), path );
        return( false );
    }

    int start_id = 1;

    for( int n = 0; n < csv.n_fields(); n++ )
    {
        csv_reader_line_info_c *ff = csv.get_field( n );

        if( ff->get_desc().Length() > 63 )
        {
            CEikonEnv::Static()->InfoWinL( _L("Error, field description larger than 64 !"), ff->get_desc() );
            return (false);
        }

        dbm_field_type_e type = DFT_TEXT;

        TBuf <64> name;
        name.Copy( ff->get_desc() );


        if( ff->get_type_info() != TChar('T') )
        {
            TChar tt = ff->get_type_info();

            if( tt == TChar('I') )
            {
                type = DFT_INT;
            }
            if( tt == TChar('D') )
            {
                type = DFT_DATE;
            }
            if( tt == TChar('M') )
            {
                type = DFT_TIME;
            }
            if( tt == TChar('P') )
            {
                type = DFT_PHONE_NUMBER;
            }

            if( tt == TChar('F') )
            {
                type = DFT_FLOAT;
            }

            if( tt == TChar('L') )
            {
                type = DFT_LIST;
            }
        }

        file->insert_field_ex( n + start_id , name , type , 64 );
    }


    return ( true );
}

bool db_import_export_c::do_import_csv_ex( TPtrC path )
{
  csv_reader_c *pcsv = new csv_reader_c( separators[ current_separator ] );

  if( !pcsv->start( path ) )
  {
      CEikonEnv::Static()->InfoWinL( _L("Error, cannot read"), path );
      return( false );
  }

  RArray <csv_reader_line_info_c *> fields_sources;
  bool at_least_one_is_ok = false;
  for( int n = 0; n < file->n_fields(); n++ )
  {
      TPtrC name = file->get_field( n )->name();
      csv_reader_line_info_c *src = pcsv->find_field(name );

      if( src )
          at_least_one_is_ok = true;

      fields_sources.Append( src ); // ATT. might be null !
  }

  if( !at_least_one_is_ok )
  {
      CEikonEnv::Static()->InfoWinL( *strTextInvalidFormat , _L("") );

      fields_sources.Close();
      return ( false );
  }
  file->get_table().Reset();

  iProgressDialog = new(ELeave)CAknProgressDialog( (REINTERPRET_CAST(CEikDialog**,&iProgressDialog)));
  CEikProgressInfo* iProgressInfo = iProgressDialog->GetProgressInfoL();
  iProgressInfo->SetFinalValue( 100 );
//  iProgressInfo->SetTone( CAknNoteDialog::EConfirmationTone );
  iProgressDialog->ExecuteLD(R_PROGRESS_NOTE);
}

void db_import_export_c::DialogDismissedL (TInt aButtonId)
{

}

bool db_import_export_c::do_import_csv( TPtrC path )
{
/*
    CAknWaitDialog *waitDialog;
    waitDialog = new (ELeave) CAknWaitDialog(NULL, ETrue);
    waitDialog->ExecuteLD(R_AKNEXNOTE_WAIT_NOTE_SOFTKEY_CANCEL);
*/

    csv_reader_c csv( separators[ current_separator ] );

    if( !csv.start( path ) )
    {
        CEikonEnv::Static()->InfoWinL( _L("Error, cannot read"), path );
        return( false );
    }

    RArray <csv_reader_line_info_c *> fields_sources;
    bool at_least_one_is_ok = false;
    for( int n = 0; n < file->n_fields(); n++ )
    {
        TPtrC name = file->get_field( n )->name();
        csv_reader_line_info_c *src = csv.find_field(name );

        if( src )
            at_least_one_is_ok = true;

        fields_sources.Append( src ); // ATT. might be null !
    }

    if( !at_least_one_is_ok )
    {
        CEikonEnv::Static()->InfoWinL( *strTextInvalidFormat , _L("") );

        fields_sources.Close();
        return ( false );
    }

    db_data_item_c db_item;

    file->get_table().Reset();

    while( csv.goto_next_line() )
    {
        file->get_table().InsertL();

        // Implement code to handle lines here
        for( int i = 0; i < fields_sources.Count(); i++ )
        {
            if( !fields_sources[i] )
                    continue;

            db_item.workout_from_str( file->get_field(i), fields_sources[i]->get_text() );
            db_item.to_db( file->get_field(i), file );
        }

        file->get_table().PutL();

        undo_bookmarks.Append( file->get_table().Bookmark() );
    }

    csv.end();

    fields_sources.Close();

//    waitDialog->ProcessFinishedL(); // deletes the dialog

    return ( true );
}

bool db_import_export_c::undo_import()
{
    for( int n = 0; n < undo_bookmarks.Count(); n++ )
    {
        TDbBookmark amark =undo_bookmarks[n];

        file->get_table().GotoL( amark );
        file->get_table().DeleteL();
    }

    undo_bookmarks.Reset();

    return ( true );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifndef _NO_INTERFACE_PLEASE_

#ifndef __NO_COMMON_DIALOGS__

#include <aknlistquerydialog.h>
#include <akncommondialogs.h>

bool db_import_export_c::do_export_csv()
{
    if( !file )
            return (false);

    TBuf <256> fname;
    fname.Append( file->name() );
    fname.Append( _L(".CSV") );

    AknCommonDialogs obj;
    bool ret = obj.RunSaveDlgLD(  fname,R_DATA_VIEW_CSV_MEMORY_SELECTION_DIALOG );

    if( ret  )
    {
        ret = do_export_csv( fname );

        if( ret )
        {
                CEikonEnv::Static()->InfoMsg( R_DB_IMPORT_EXPORT_STR_DONE );
        }
    }

    return (ret);
}

bool db_import_export_c::do_import_csv()
{
    TBuf <256> fname;
    AknCommonDialogs obj;
    bool ret = obj.RunSelectDlgLD( fname,R_DATA_VIEW_CSV_MEMORY_SELECTION_DIALOG, R_DATA_VIEW_CSV_SELECTION_DIALOG );

    if( ret )
    {
        ret = do_import_csv( fname );

        if( ret )
        {
                CEikonEnv::Static()->InfoMsg( R_DB_IMPORT_EXPORT_STR_DONE );
        }

    }

    return (ret);
}

bool db_import_export_c::do_import_structure()
{
    TBuf <256> fname;
    AknCommonDialogs obj;
    bool ret = obj.RunSelectDlgLD( fname,R_DATA_VIEW_CSV_MEMORY_SELECTION_DIALOG, R_DATA_VIEW_CSV_SELECTION_DIALOG );

    if( ret )
    {
        return do_import_structure( fname );
    }

    return (false);

}
#else


bool db_import_export_c::do_export_csv()
{
    if( !file )
            return (false);

    TBuf <256> fname;
    fname.Append( file->name() );
    fname.Append( _L(".CSV") );

//    AknCommonDialogs obj;
//    bool ret = obj.RunSaveDlgLD(  fname,R_DATA_VIEW_CSV_MEMORY_SELECTION_DIALOG );

    file_query_dialog_c dlg;
    bool ret = dlg.do_it_for_save( fname );

    if( ret  )
    {
        ret = do_export_csv( fname );

        if( ret )
        {
                CEikonEnv::Static()->InfoMsg( R_DB_IMPORT_EXPORT_STR_DONE );
        }
    }

    return (ret);
}

bool db_import_export_c::do_import_csv()
{
    TBuf <256> fname;

    file_query_dialog_c dlg;
    bool ret = dlg.do_it( fname );

    if( ret )
    {
        ret = do_import_csv( fname );

        if( ret )
        {
                CEikonEnv::Static()->InfoMsg( R_DB_IMPORT_EXPORT_STR_DONE );
        }

    }

    return (ret);
}

bool db_import_export_c::do_import_structure()
{
    TBuf <256> fname;

    file_query_dialog_c dlg;
    bool ret = dlg.do_it( fname );

    if( ret )
    {
        return do_import_structure( fname );
    }

    return (false);

}

#endif

TChar db_import_export_c::select_separator()
{
    TInt index( 0 );

    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_DB_IMPORT_EXPORT_SEPARATOR_QUERY );
    TInt answer = dlg->RunLD();

    if( answer )
    {
        current_separator = index;
    }

    return separators[ current_separator ];
}

#endif

