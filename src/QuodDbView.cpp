/*------------------------------------------------------------------------------

                    Main View for the QuodDb APP

JCR 3.2004
------------------------------------------------------------------------------*/


#include "QuodDbView.h"
#include "quodDbMaster.h"
#include <quoddb.rsg>
#include "QuodDb.hrh"
#include "DbMaster.h"


#include "QuodDbFieldFormDef.h"

#include <eikmenup.h>						// CEikMenuPane
#include <eikbtgpc.h>

#include <aknviewappui.h>					// CAknViewAppUi
#include <aknselectionlist.h>
#include <aknquerydialog.h>

#include <avkon.rsg>
#include <avkon.hrh>
#include <akntitle.h>

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CQuodDbView* CQuodDbView::NewL( quod_db_main_c *main_app )
{
	CQuodDbView* self = CQuodDbView::NewLC( main_app );
	CleanupStack::Pop(self);
	return self;
}

CQuodDbView* CQuodDbView::NewLC( quod_db_main_c *main_app)
{
	CQuodDbView* self = new (ELeave) CQuodDbView( main_app );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CQuodDbView::CQuodDbView( quod_db_main_c *_main_app ):observer( this )
{
    main_app = _main_app;
    iContainer = 0;
}

CQuodDbView::~CQuodDbView()
{

}

void CQuodDbView::ConstructL()
{
	BaseConstructL( R_DATA_DEFINITION_VIEW );
}

TUid CQuodDbView::Id() const
{
	return TUid::Uid( EDataDefinitionViewId );
}

/**
* Called by the framework when the view is activated.  Constructs the container if necessary, setting this view as its MOP parent, and
 adding it to the control stack.
*/
void CQuodDbView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,	 const TDesC8& /*aCustomMessage*/)
{

	if (!iContainer)
    {
        iContainer = new (ELeave) list_box_container_c( &observer );
        iContainer->SetMopParent(this); // ATT: Mudar a ordem deste gajo faz desaparecer as setas !!!
        iContainer->ConstructL( ClientRect() );

		AppUi()->AddToStackL(*this, iContainer);

        quod_db_master_c *dbm = MainApp()->get_db_master();

        if( dbm->get_current_db() )
        {
            ChangeMode( QVM_TABLE_LIST );
        }
        else
        {
            ChangeMode( QVM_DATABASE_LIST ); // By Now force to start at a database, in the future start at user options !
        }
	}
}

/**
* Called by the framework when the view is deactivated.Removes the container from the control stack and deletes it.
*/
void CQuodDbView::DoDeactivate()
{
//    MainApp()->will_exit(); CANNOT BE DONE HERE CAUSE IT WILL BE CALLED WHEN MOVING TO TEXT WINDOW !!!!

	if (iContainer)
	{
		AppUi()->RemoveFromStack(iContainer);
		delete iContainer;
		iContainer = NULL;
	}

}

void CQuodDbView::HandleCommandL(TInt aCommand)
{

	switch (aCommand)
    {
        /*
        case EMenuEditData:
        {
            SomethingEditData();
        }
        break;
        */

        case EMenuNew:
        {
            SomethingNew();
        }
        break;


        case EMenuSelect:
        {
             SomethingSelected();
        }
        break;

        case EMenuDelete:
        {
            SomethingToDelete();
        }
        break;

        case EMenuChangeDB:
        {
            ChangeMode( QVM_DATABASE_LIST );
        }
        break;

        case EMenuChangeTable:
        {
            ChangeMode( QVM_TABLE_LIST );
        }
        break;

        case EMenuExternalImport:
        {
            ImportCsvStructure();
        }
        break;

        case EMenuExternalSeparator:
        {
            import_export.select_separator();
        }
        break;



        case EAknSoftkeyBack:
        {
              main_app->switch_mode( QMV_MODE_MAIN );
              return;
        }

		case EAknSoftkeyExit:
        {
            MainApp()->will_exit();

			AppUi()->HandleCommandL(EEikCmdExit);

        }
        break;

		default:
        {
			AppUi()->HandleCommandL(aCommand);
        }
    }
}

void CQuodDbView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
    if (aResourceId != R_DATA_DEFINITION_VIEW_MENU )
            return;

    // No Select !
    if( iContainer && iContainer->GetNumListItems() == 0 )
    {
        aMenuPane->SetItemDimmed( EMenuSelect ,ETrue);
        aMenuPane->SetItemDimmed( EMenuDelete ,ETrue);
    }



    // TURN OFF UNWANTED OPTIONS
   switch( MainApp()->get_mode() )
    {
        case QVM_DATABASE_LIST:
        {
            aMenuPane->SetItemDimmed(EMenuChangeDB,ETrue);
            aMenuPane->SetItemDimmed(EMenuChangeTable,ETrue);
            aMenuPane->SetItemDimmed(EMenuExternal,ETrue);

        }
        break;


        case QVM_TABLE_LIST:
        {
            aMenuPane->SetItemDimmed(EMenuChangeTable,ETrue);
            aMenuPane->SetItemDimmed(EMenuExternal,ETrue);
        }
        break;

        case QVM_TABLE_EDIT:
        {

        }
        break;
    default:break;
    }

}


void CQuodDbView::ChangeMode( QuodDataEdition_Modes_e new_mode )
{
    main_app->set_mode( new_mode );

//   TPtr *extra = 0;
    quod_db_master_c *dbm = MainApp()->get_db_master();

   switch( MainApp()->get_mode() )
    {
        case QVM_DATABASE_LIST: ChangeTitleText( R_TITLE_DATABASE_LIST ); break;

        case QVM_TABLE_LIST:
                {
                    if( dbm->get_current_db() )
                        ChangeTitleText( R_TITLE_TABLES_LIST , dbm->get_current_db()->name() );
                }
                break;

        case QVM_TABLE_EDIT:
                {
                    if( dbm->get_current_file() )
                        ChangeTitleText( R_TITLE_COLUMNS_LIST ,dbm->get_current_file()->name() );
                }
                break;
        default:break;
    }

    UpdateContainer();
}

void CQuodDbView::UpdateContainer()
{
    if (!iContainer)
                return;

    TBuf <256> stunt;
    CDesCArrayFlat* tArray = 0;
    list_box_container_types_e new_type = LBCT_NO_HEADER;

    quod_db_master_c *dbm = MainApp()->get_db_master();

    switch( main_app->get_mode() )
    {
        case QVM_DATABASE_LIST:
        {
            if( !dbm->n_databases() )
                    return;

            tArray = new (ELeave) CDesCArrayFlat(10);

            for( int n = 0; n < dbm->n_databases(); n++ )
            {
                stunt.Zero();

                TChar tab('\t');
                stunt.Append( tab );
                stunt.Append( dbm->database_name( n ) );

                tArray->AppendL( stunt );
            }

        }
        break;

        case QVM_TABLE_LIST:
        {
            new_type = LBCT_NUMERIC_HEADER;
            if( dbm->get_current_db() && dbm->get_current_db()->n_files() )
            {
                dbm_master_c *database = dbm->get_current_db();

                tArray = new (ELeave) CDesCArrayFlat( database->n_files() );

                for( int n = 0; n < database->n_files(); n++ )
                {
                    stunt.Zero();

                    TChar tab('\t');
                    stunt.AppendNum( database->get_file(n)->id() );
                    stunt.Append( tab );
                    stunt.Append( database->get_file(n)->name() );

                    tArray->AppendL( stunt );
                }
            }
        }
        break;

        case QVM_TABLE_EDIT:
        {
                new_type = LBCT_NUMERIC_HEADER;
                if( dbm->get_current_db() && dbm->get_current_file() )
                {
                    dbm_file_c *afile = dbm->get_current_file();

                    tArray = new (ELeave) CDesCArrayFlat(10);

                    for( int n = 0; n < afile->n_fields(); n++ )
                    {
                        stunt.Zero();

                        TChar tab('\t');
                        stunt.AppendNum( afile->get_field(n)->id() );
                        stunt.Append( tab );
                        stunt.Append( afile->get_field(n)->name() );

                        tArray->AppendL( stunt );
                    }

                }
        }
        break;

        case QVM_TABLE:
        {
            new_type = LBCT_HEADER;

            tArray = new (ELeave) CDesCArrayFlat(10);
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));

        }
        break;
        default:break;
    }

    iContainer->Rebuild( new_type , tArray, ELbmOwnsItemArray );
}

void CQuodDbView::SomethingNew()
{
    TBuf<255> textData(NULL);
    quod_db_master_c *dbm = MainApp()->get_db_master();

    // Create database or table ? must get a name ...
    switch( MainApp()->get_mode() )
    {
        case QVM_DATABASE_LIST:
        {
            HBufC* strText = iEikonEnv->AllocReadResourceL( R_NEW_DATABASE_NAME );
            TBuf <1024> text( *strText );

            CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog( textData, CAknQueryDialog::ENoTone );
            if (dlg->ExecuteLD( R_QUODB_DATA_QUERY_DIALOG_DB , text ))
            {
                if( !dbm->valid_db_name( textData ) )
                {
                    CEikonEnv::Static()->InfoWinL( R_STR_ERROR_INVALID_NAME );
                    delete strText;
                    return;
                }

                dbm_master_c *db = dbm->new_database( textData );

                delete db; // We don't wan't it by now ...
            }

            delete strText;
        }
        break;

        case QVM_TABLE_LIST:
        {
            dbm_file_c *file = 0;

            dbm_master_c *db = dbm->get_current_db();

            if( db )
            {
                HBufC* strText = iEikonEnv->AllocReadResourceL( R_NEW_TABLE_NAME );
                TBuf <1024> text( *strText );

                CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog( textData, CAknQueryDialog::ENoTone );
                if (dlg->ExecuteLD( R_QUODB_DATA_QUERY_DIALOG_DB , text ))
                {
                    if( !db->valid_file_name( textData ) )
                    {
                        CEikonEnv::Static()->InfoWinL( R_STR_ERROR_INVALID_NAME );
                        delete strText;
                        return;
                    }

                    int last_id = 0;

                    if( db->n_files() )
                          last_id = db->get_file( db->n_files()-1 )->id();

                    file = new dbm_file_c( last_id +1, textData );

                    if( file )
                    {
                        file->init( db );
                        db->insert_file( file );
                    }
                }

                delete strText;

            }
        }
        break;

        case QVM_TABLE_EDIT:
        {
                dbm_file_c *afile = dbm->get_current_file();

                if( dbm->get_current_db() && afile )
                {
                    TBufC <FIELD_SIZE> nome;
                    int next_id = afile->get_next_empty_field_id();

                    dbm_field_c *field = new dbm_field_c( next_id, nome, DFT_TEXT, 64 );

                    CQuodDbFieldFormDef *form = CQuodDbFieldFormDef::NewL( afile, field );
                    form->PrepareLC( R_FIELD_FORM_DEF_DIALOG );
                    form->RunLD();

                    //afile->insert_field( field ); INSERTED IN THE FORM

                }
        }
        break;
        default:break;
    }

    UpdateContainer();
}

void CQuodDbView::SomethingToDelete()
{
    if (!iContainer)
            return;

    int selected_idx = iContainer->GetCurrentItemIdx();

    if( selected_idx < 0 )
            return;

    quod_db_master_c *dbm = MainApp()->get_db_master();

    // Create database or table ? must get a name ...
    switch( MainApp()->get_mode() )
    {
        case QVM_DATABASE_LIST:
        {
            if( dbm->database_name( selected_idx ) == dbm->get_current_db()->name() )
            {
                CEikonEnv::Static()->AlertWin( _L("Sorry, cannot delete"), _L("the current database.") );
                break;
            }

            HBufC* strText = iEikonEnv->AllocReadResourceL( R_CONFIRM_DELETE_DB );
            TBuf <1024> text( *strText );
            text.Append( dbm->database_name( selected_idx ) ); // Dialog DBs idxs correspond to dbm ones ...

            CAknQueryDialog *dlg = new CAknQueryDialog();
            if (dlg->ExecuteLD( R_QUODB_CONFIRMATION_YES_NO_DLG, text ) )
            {
                if( dbm->delete_database( selected_idx ) == false )
                {
                    CEikonEnv::Static()->AlertWin( _L("Error deleting database"), dbm->database_name( selected_idx ) );
                }
                else
                {
                    dbm->init(); // To update the db list ...
                }
            }

            delete strText;

        }
        break;

        case QVM_TABLE_LIST:
        {
            if( dbm->get_current_db() )
            {
                dbm_master_c *db = dbm->get_current_db();

                HBufC* strText = iEikonEnv->AllocReadResourceL( R_CONFIRM_DELETE_TABLE );
                TBuf <1024> text( *strText );
                text.Append( db->get_file( selected_idx )->name() ); // Dialog DBs idxs correspond to dbm ones ...

                CAknQueryDialog *dlg = new CAknQueryDialog();
                if (dlg->ExecuteLD( R_QUODB_CONFIRMATION_YES_NO_DLG, text ) )
                {
                    if( db->delete_file( selected_idx ) == false )
                    {
                        CEikonEnv::Static()->AlertWin( _L("Error deleting table"), db->get_file( selected_idx )->name() );
                    }
                }

                delete strText;
            }
        }
        break;

        case QVM_TABLE_EDIT:
        {
            if( dbm->get_current_file() )
            {
                dbm_file_c *file = dbm->get_current_file();

                HBufC* strText = iEikonEnv->AllocReadResourceL( R_CONFIRM_DELETE_COLUMN );
                TBuf <1024> text( *strText );
                text.Append( file->get_field( selected_idx )->name() ); // Dialog DBs idxs correspond to dbm ones ...

                CAknQueryDialog *dlg = new CAknQueryDialog();
                if (dlg->ExecuteLD( R_QUODB_CONFIRMATION_YES_NO_DLG, text ) )
                {
                    if( file->delete_field( selected_idx ) == false )
                    {
                        CEikonEnv::Static()->AlertWin( _L("Error deleting column"), file->get_field( selected_idx )->name() );
                    }
                }

                delete strText;
            }

        }
        break;
    default:break;
    }


    UpdateContainer();
}

void CQuodDbView::SomethingSelected()
{
    if (!iContainer)
            return;

    int selected_idx = iContainer->GetCurrentItemIdx();

    if( selected_idx < 0 )
            return;

    quod_db_master_c *dbm = MainApp()->get_db_master();

    // Create database or table ? must get a name ...
    switch( MainApp()->get_mode() )
    {
        case QVM_DATABASE_LIST:
        {
            delete dbm->get_current_db();

            dbm_master_c *db = dbm->open_database( selected_idx );

            dbm->set_current_db( db );

            ChangeMode( QVM_TABLE_LIST );


        }
        return;

        case QVM_TABLE_LIST:
        {
            dbm_master_c *db = dbm->get_current_db();

            if( db )
            {
                 dbm_file_c *file = db->get_file( selected_idx );

                 MainApp()->get_db_master()->set_current_file( file );

                 ChangeMode( QVM_TABLE_EDIT );
            }
        }
        return;

        case QVM_TABLE_EDIT:
        {
            dbm_master_c *db = dbm->get_current_db();
            dbm_file_c *file = dbm->get_current_file();

            if( db && file && selected_idx < file->n_fields() )
            {
                dbm_field_c *field = file->get_field( selected_idx );

                CQuodDbFieldFormDef *form = CQuodDbFieldFormDef::NewL( file,field );
                form->PrepareLC( R_FIELD_FORM_DEF_DIALOG );
                form->RunLD();

                UpdateContainer();
            }
        }
        return;

        default:break;
    }

    UpdateContainer();
}

void CQuodDbView::SomethingEditData()
{
    int selected_idx = iContainer->GetCurrentItemIdx();

    if( selected_idx < 0 )
            return;

    quod_db_master_c *dbm = MainApp()->get_db_master();
    dbm_master_c *db = dbm->get_current_db();

    if( db )
    {
         dbm_file_c *file = db->get_file( selected_idx );

         MainApp()->get_db_master()->set_current_file( file );

         // IF the table is created goes to data mode, if not to edit table ..
         if( file->n_fields() )
         {
                 MainApp()->switch_mode( (QuodMainView_Modes_e) 1 );
         }
         else
         {
                HBufC* strText = iEikonEnv->AllocReadResourceL( R_STR_MUST_DEFINE_TABLE_COLUMNS );
                TBuf <1024> text( *strText );

                CAknQueryDialog *dlg = new CAknQueryDialog();
                if (dlg->ExecuteLD( R_QUODB_CONFIRMATION_YES_NO_DLG, text ) )
                {
                    SomethingSelected();
                }

                delete strText;
         }
    }
}

void CQuodDbView::ChangeTitleText( int resource_id , TPtrC extra )
{
    CAknTitlePane* titlePane = STATIC_CAST(
        CAknTitlePane*,
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    if ( !resource_id )
    {
        titlePane->SetTextToDefaultL();
    }
    else
    {
        TBuf<1024> titleText( NULL );
        iEikonEnv->ReadResource( titleText, resource_id );

        if( extra.Length() )
        {
            titleText.Append( extra );
        }

        titlePane->SetTextL( titleText );
    }
}

void CQuodDbView::ImportCsvStructure()
{
      quod_db_master_c *dbm = MainApp()->get_db_master();

      dbm_master_c *db = dbm->get_current_db();
      dbm_file_c *file = dbm->get_current_file();

      if(!db || !file )
            return;

      import_export.set_file( file );
      import_export.do_import_structure();

      UpdateContainer();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CQuodDbView_list_box_container_enhanced_observer_c::CQuodDbView_list_box_container_enhanced_observer_c( CQuodDbView *_daddy )
{
    daddy = _daddy;
}

void CQuodDbView_list_box_container_enhanced_observer_c::HandleListBoxEventL( CEikListBox* aListBox, MEikListBoxObserver::TListBoxEvent aListBoxEvent )
{

};

TKeyResponse CQuodDbView_list_box_container_enhanced_observer_c::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType == EEventKey)
    {
        switch (aKeyEvent.iCode)
        {
            case EKeyDevice3: // OK
              {
                daddy->SomethingSelected();
                return EKeyWasConsumed;
              }

            default:
                break;
        }
    }

    return EKeyWasNotConsumed;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// End of File
