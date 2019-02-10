/*------------------------------------------------------------------------------

                    DB Field Selector Dialog

  JCR 6.2004
------------------------------------------------------------------------------*/
#ifndef DB_FIELD_SELECTOR_DLG_H
#define DB_FIELD_SELECTOR_DLG_H

#include <e32std.h>
#include "dbMaster.h"
#include "dbDataItem.h"

#include <akniconarray.h> // CAknIconArray

class db_field_selector_dlg_c
{
    RArray <TDbBookmark> bookmarks;

    CDesCArrayFlat *build_field_array( dbm_file_c *file, dbm_field_c *field_heading, dbm_field_c *field );

    db_data_item_c selected_field;
    db_data_item_c selected_heading;

    CArrayPtr<CGulIcon> *icon_array;

    public:
    db_field_selector_dlg_c();
    ~db_field_selector_dlg_c();


    bool do_it( dbm_file_c *file, dbm_field_c *field_heading, dbm_field_c *field );

    db_data_item_c *get_selected_field()
    {
        return &selected_field;
    }

    db_data_item_c *get_selected_heading()
    {
        return &selected_heading;
    }

    void set_icon_array( CArrayPtr<CGulIcon> *_icon_array )
    {
        icon_array = _icon_array;
    }
};

#endif
