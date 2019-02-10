/*------------------------------------------------------------------------------

                        Main Module

                 Keeps Global APP Information

    JCR 3.2003
------------------------------------------------------------------------------*/
#ifndef QUODDBMAIN_H
#define QUODDBMAIN_H

#include "quodDbMaster.h"
#include "dataView.h"
#include "genericCfg.h"

class CCoeAppUi;
class data_view_c;
class CAknViewAppUi;
class CAknView;

enum QuodMainView_Modes_e
{
    QMV_MODE_MAIN = 1,
    QMV_MODE_DATA_DEF,
    QMV_MODE_DATA_EDIT,
    QMV_MODE_DATA_REPORT
};

enum QuodDataEdition_Modes_e
{
    QVM_NONE,
    QVM_DATABASE_LIST,
    QVM_TABLE_LIST,
    QVM_TABLE_EDIT,
    QVM_TABLE,
};

class quod_db_main_c;

class my_data_view_handler_c : public data_view_handler_c
{
    quod_db_main_c *main_app;

public:
    my_data_view_handler_c( quod_db_main_c *_main_app )
    {
        main_app = _main_app;
    }

    virtual bool HandleCommandL(data_view_c *dv, TInt aCommand);
//    virtual int gimme_a_resource_id( data_view_resources_e res );
};


class quod_db_main_c: public generic_cfg_base_c
{
      quod_db_master_c db_master;
      QuodDataEdition_Modes_e current_mode;
      CAknViewAppUi *app_ui;
      TUid data_view_id;
      TUid main_view_id;
      TUid data_def_view_id;
      TUid current_view_id;

      my_data_view_handler_c data_view_handler;

      bool cfgs_loaded;

  public:
      quod_db_main_c( CAknViewAppUi *app_ui );
      ~quod_db_main_c();

      bool init();
      bool load_cfgs();

      quod_db_master_c *get_db_master();

      QuodDataEdition_Modes_e get_mode()
      {
            return current_mode;
      }

      void set_mode( QuodDataEdition_Modes_e mode )
      {
            current_mode = mode;
      }

      void will_exit();

      void set_data_view( TUid _data_view );
      void set_main_view( TUid _main_view );
      void set_data_def_view( TUid _id );

      void switch_mode( QuodMainView_Modes_e mode );

      my_data_view_handler_c *get_data_view_handler()
      {
            return &data_view_handler;
      }

      TUid get_current_view_id()
      {
          return current_view_id;
      }

      TUid get_main_view_id()
      {
            return main_view_id;
      }

      void ExternalizeL(RWriteStream& aStream) const;
      void InternalizeL(RReadStream& aStream);
};

#endif