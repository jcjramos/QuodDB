/*------------------------------------------------------------------------------

                        Main Module

                 Keeps Global APP Information

    JCR 3.2003
------------------------------------------------------------------------------*/
#include "QuodDb.hrh" // commands
#include <QuodDb.rsg>

#include "dbmaster.h"
#include "quoddbmaster.h"
#include "quoddbmain.h"

#include "dataview.h"
#include <aknviewappui.h>


_LIT( K_Cfg_file,"QuodDB" );
_LIT( K_Cfg_file_ext,".CFG" );
_LIT( K_Cfg_file_identifier,"[QuodDB MAIN CFG FILE]" );
_LIT( K_Default_Database_name,"default" );

const int QUODDB_CURRENT_CFG_FILE_VERSION = 1;


quod_db_main_c::quod_db_main_c( CAknViewAppUi *_app_ui )
    :app_ui(_app_ui),data_view_handler( this ),db_master( _app_ui )
{
    current_mode = QVM_NONE;
    cfgs_loaded = false;
}

quod_db_main_c::~quod_db_main_c()
{

}

bool quod_db_main_c::init()
{
    return db_master.init();
}

bool quod_db_main_c::load_cfgs()
{
    if( !cfgs_loaded )
    {
        TBufC <255> file( K_Cfg_file );
        TBufC <255> ext( K_Cfg_file_ext );

        generic_cfg_c cfg( this );
        if( !cfg.load( db_master.get_db_path(), file, ext ) )
        {
            // FIRST TIME ? LETZ CREATE DEFAULT DB
            TBufC <256> dbn( K_Default_Database_name );
            dbm_master_c *db = db_master.new_database( dbn );
            db_master.set_current_db( db );
        }

        cfgs_loaded = true;

        return (true);
    }
    else
    {
        return (false);
    }
}

quod_db_master_c *quod_db_main_c::get_db_master()
{
   return &( db_master );
}

void quod_db_main_c::will_exit()
{
    TBufC <255> file( K_Cfg_file );
    TBufC <255> ext( K_Cfg_file_ext );

    generic_cfg_c cfg( this );
    cfg.save( db_master.get_db_path(), file, ext );

    // Reset current DB!
    delete db_master.get_current_db();
    db_master.set_current_db( 0 );
    db_master.set_current_file( 0 );
}

void quod_db_main_c::set_data_view( TUid _data_view )
{
    data_view_id = _data_view;
}

void quod_db_main_c::set_main_view( TUid _main_view )
{
    main_view_id = _main_view;
}

void quod_db_main_c::set_data_def_view( TUid _id )
{
    data_def_view_id = _id;
}

void quod_db_main_c::switch_mode( QuodMainView_Modes_e mode )
{
    switch( mode )
    {
        case QMV_MODE_MAIN:
            {
                current_view_id = main_view_id;
                app_ui->ActivateLocalViewL( main_view_id );
            }
            break;

        case QMV_MODE_DATA_DEF:
            {
                current_view_id = data_def_view_id;
                app_ui->ActivateLocalViewL( data_def_view_id );
            }
            break;

        case QMV_MODE_DATA_REPORT:
        case QMV_MODE_DATA_EDIT:
            {
                 current_view_id = data_view_id;
                 app_ui->ActivateLocalViewL( data_view_id );

                 CAknView *view = app_ui->View( data_view_id );
                 ((data_view_c*)view)->get_database_manipulator().prepare_for_db( db_master.get_current_db(), db_master.get_current_file() );

                 if( mode == QMV_MODE_DATA_REPORT )
                          ((data_view_c*)view)->set_edit_mode( 0 );
                      else
                          ((data_view_c*)view)->set_edit_mode( 1 );

            }
            break;
    }
}

void quod_db_main_c::ExternalizeL( RWriteStream& aStream ) const
{
    TBufC <256> idt( K_Cfg_file_identifier );
    TBufC <256> empty;

    GC_WRITE_STR( idt );
    aStream.WriteInt32L( QUODDB_CURRENT_CFG_FILE_VERSION );

    if(db_master.current_db )
    {
        GC_WRITE_STR( db_master.current_db->name() );
    }
    else
    {
        GC_WRITE_STR( empty );
    }

    if( db_master.current_file )
    {
        GC_WRITE_STR( db_master.current_file->name() );
    }
    else
    {
        GC_WRITE_STR( empty );
    }

}

void quod_db_main_c::InternalizeL( RReadStream& aStream )
{
      TBuf <256> header;
      TBuf <256> stunt_des;

      int len = aStream.ReadInt32L();
      aStream.ReadL( header, len );

      if( header != K_Cfg_file_identifier ) // Invalid file !
            return;

      int version = aStream.ReadInt32L();

      if( version > QUODDB_CURRENT_CFG_FILE_VERSION )
           return;

     GC_READ_STR( stunt_des );

     dbm_master_c *db = 0;
     if( stunt_des.Length() )
     {
        db = db_master.open_database( stunt_des );
        db_master.set_current_db( db );
     }

     GC_READ_STR( stunt_des );

     if( db && stunt_des.Length() )
     {
        dbm_file_c *file = db->find_file( stunt_des );
        if( file )
            db_master.set_current_file( file );
     }

}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


bool my_data_view_handler_c::HandleCommandL(data_view_c *dv, TInt aCommand)
{
    if( aCommand == EAknSoftkeyBack )
    {
        main_app->switch_mode( QMV_MODE_MAIN );
        
        return ( false );
    }
    else
    {
        return ( false );
    }
}
/*DEPRECATED !!!!
int my_data_view_handler_c::gimme_a_resource_id( data_view_resources_e res )
{
    switch( res )
    {
        case E_DVR_VIEW:return R_DATA_VIEW;
        case E_DVR_FORM:return R_DATA_VIEW_FIELD_EDIT_FORM_DIALOG;
        case E_DVR_FORM_NON_EDITABLE:return R_DATA_VIEW_FIELD_EDIT_FORM_DIALOG_NON_EDITABLE;
        case E_DVR_FORM_POPUP:return R_DATA_VIEW_FIELD_EDIT_FORM_POPUP;
        case E_DVR_FORM_POPUP_TEXT:return R_DATA_VIEW_FIELD_EDIT_FORM_TEXT;
        case E_DVR_FORM_DATE:return R_DATA_VIEW_FIELD_EDIT_FORM_DATE_EDITOR;
        case E_DVR_FORM_TIME:return R_DATA_VIEW_FIELD_EDIT_FORM_TIME_EDITOR;
        case E_DVR_STR_CONFIRM_DELETE_RECORD:return R_DATA_VIEW_CONFIRM_DELETE;
        case E_DVR_YES_NO_DIALOG:return R_DATA_VIEW_CONFIRMATION_YES_NO_DLG;
        case E_DVR_MAIN_MENU:return R_DATA_VIEW_MAIN_MENU;
        case E_DVR_LIST_QUERY:return R_DATA_VIEW_LIST_QUERY;
        case E_DVR_LIST_QUERY_MULTISEL:return R_DATA_VIEW_LIST_QUERY_MULTISEL;
        case E_DVR_STR_HEADER:return R_DATA_VIEW_STR_HEADER;
        case E_DVR_STR_SORT:return R_DATA_VIEW_STR_SORT;
        case E_DVR_STR_NONE:return R_DATA_VIEW_STR_NONE;
        case E_DVR_STR_YES_NO:return R_FIELD_ARRAY_YES_NO;
        case E_DVR_STR_DATA_REPORT:return R_DATA_VIEW_STR_DATA_REPORT;
        case E_DVR_STR_DATA_EDITION:return R_DATA_VIEW_STR_DATA_EDITION;
        case E_DVR_STR_PAGES:return R_DATA_VIEW_STR_PAGES;

        default:
            CEikonEnv::Static()->InfoMsg( _L("my_data_view_handler_c::gimme_a_resource_id ERROR") );
            break;

    }

    return ( 0 );
}
*/


