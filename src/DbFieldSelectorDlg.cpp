/*------------------------------------------------------------------------------

                    DB Field Selector Dialog

  JCR 6.2004
------------------------------------------------------------------------------*/
#include "DbFieldSelectorDlg.h"
#include <aknselectionlist.h>

#include "genAppRes.h"

#ifndef __GEN_APP_RESOURCE_INCLUDE_FILE__
#error  Please create a GenAppRes.H file including the AppName.RSH & RSH files
#error  Then include dataView.RSS in your application resource file!
#endif

// TESTE
//#include <stringloader.h>
//#include <quodcrm.mbg> // contains icon enumeration

db_field_selector_dlg_c::db_field_selector_dlg_c()
{
    icon_array = 0;
}

db_field_selector_dlg_c::~db_field_selector_dlg_c()
{
    bookmarks.Close();
}

CDesCArrayFlat *db_field_selector_dlg_c::build_field_array( dbm_file_c *file, dbm_field_c *field_heading, dbm_field_c *field )
{
    CDesCArrayFlat *tArray = new CDesCArrayFlat( 100 );
    db_data_item_c data_item;

    if( field->is_key() ) // Sort !
    {
        file->get_table().SetIndex( field->name() );
    }

    TBuf <2048> line;
    TChar tab('\t');

    for (file->get_table().FirstL(); file->get_table().AtRow(); file->get_table().NextL())
    {
          bookmarks.Append( file->get_table().Bookmark() );

          file->get_table().GetL();

          line.Zero();

          if( icon_array )
          {
              line.Append( _L("0") );
          }
          else
          {
            data_item.build_from_db( field_heading, file );
            line.Append( data_item.get_as_str() );
          }

          line.Append( tab );

          data_item.build_from_db( field, file );
          line.Append( data_item.get_as_str() );

          tArray->AppendL( line ); // Copy rowText to resultArray
    }

    return tArray;
}

bool db_field_selector_dlg_c::do_it( dbm_file_c *file, dbm_field_c *field_heading, dbm_field_c *field )
{
    if( !file || !field )
            return ( false );

    bookmarks.Reset();

    bool close_file = false;
    if( !file->get_is_open() )
    {
        file->open( true );
        close_file = true;
    }

    CDesCArrayFlat *tArray = build_field_array( file, field_heading, field );

    if( !tArray )
            return ( false );

	// Construct and prepare the dialog
    TInt openedItem = 0;
	CAknSelectionListDialog* dialog = CAknSelectionListDialog::NewL(openedItem, tArray, 0);


    if( icon_array )
    {
        dialog->PrepareLC(R_DATA_VIEW_LISTDLG_DIALOG);
        dialog->SetIconArrayL(icon_array);
    }
    else
    {
        dialog->PrepareLC(R_DATA_VIEW_LISTDLG_DIALOG_NO_SEARCH);
    }

    // TESTE
    /*
	// Create an array of icons, reading them from the file and set them in the dialog
	HBufC* iconFileName;
	iconFileName = StringLoader::LoadLC(R_GRID_PATH_MBM);

	CArrayPtr<CGulIcon>* icons = new(ELeave) CAknIconArray(1);
	CleanupStack::PushL(icons);
	icons->AppendL(CEikonEnv::Static()->CreateIconL(*iconFileName, EMbmQuodcrmClients_table, EMbmQuodcrmClients_table_mask ));

	CleanupStack::Pop(icons);
	CleanupStack::PopAndDestroy(iconFileName);

    dialog->SetIconArrayL(icons); // transferring ownership of icons
    */
    // TESTE

	// Execute the dialog
	if (dialog->RunLD ())
    {

        delete tArray;

        TDbBookmark amark = bookmarks[openedItem];
        file->get_table().GotoL( amark );

        file->get_table().GetL();

        selected_heading.build_from_db( field_heading, file );
        selected_field.build_from_db( field, file );

        if( close_file )
            file->close();

        return ( true );
    }

    delete tArray;

    if( close_file )
            file->close();

    return ( false );
}


