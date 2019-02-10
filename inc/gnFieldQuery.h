/*------------------------------------------------------------------------------

                    Generic Field Query

JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef GNFIELDQUERY_H
#define GNFIELDQUERY_H

#include "dbmaster.h"
#include "dbdataItem.h"

class gn_field_query_c
{
    db_data_item_c data_item;
    TBuf <256> buff;

  public:
    gn_field_query_c();
    ~gn_field_query_c();

    bool query_field(  dbm_field_c *ff , TPtrC textData );

    db_data_item_c &get_data_item()
    {
        return data_item;
    }

};

#endif