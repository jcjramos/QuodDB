/*-------------------------------------------------------------------------------



            QuodDB master class

JCR 3.2004
------------------------------------------------------------------------------*/

#ifndef QUODDBMASTER_H
#define QUODDBMASTER_H

#include <e32std.h>
#include <badesca.h>
#include <aknviewappui.h> 

class dbm_master_c;
class dbm_file_c;

class quod_db_master_c
{
	TFileName privatePath;
    //RArray <TDesC> databases;
    TBuf <256> pata;
    CDesCArrayFlat databases;
    CAknViewAppUi *app_ui;

    dbm_master_c *current_db;
    dbm_file_c *current_file;
    
    friend class quod_db_main_c;

public:
    quod_db_master_c( CAknViewAppUi *app_ui );
    ~quod_db_master_c();

    bool init();

    int n_databases();
    TPtrC database_name( int idx );

    dbm_master_c *open_database( int idx ); // Please delete the PTR after using !
    dbm_master_c *open_database( TPtrC name ); // Please delete the PTR after using !
    dbm_master_c *new_database( TPtrC name );

    bool delete_database( TPtrC name );
    bool delete_database( int idx );

    dbm_master_c *get_current_db()
    {
        return current_db;
    }

    void set_current_db( dbm_master_c *db )
    {
        current_db = db;
        current_file = 0;
    }

    dbm_file_c *get_current_file()
    {
        return current_file;
    }

    void set_current_file( dbm_file_c *file )
    {
        current_file = file;
    }

    TPtrC get_db_path()
    {
        return pata;
    }

    bool valid_db_name( TPtrC name );

    TFileName ApplicationDriveAndPath();
};

#endif