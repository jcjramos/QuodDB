/*------------------------------------------------------------------------------

                    Class DB Data Item

JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef DBDATAITEM_H
#define DBDATAITEM_H

#include "dbmaster.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class db_data_item_c
{
    TBuf <256> buff;
    int    number;
    TTime  tm;
    TReal  rnumber;
    TInt64 bnumber;

//    CDesCArrayFlat *tYesNoArray;

public:
    db_data_item_c();
    ~db_data_item_c();

    bool build_from_db( dbm_field_c *ff , dbm_file_c *file );
    bool update_str( dbm_field_c *ff );
    bool workout_from_str( dbm_field_c *ff , TPtrC str );
    bool to_db( dbm_field_c *ff , dbm_file_c *file  );
    void reset();

    TPtrC get_as_str()
    {
        return buff;
    }

    TBuf <256> &get_as_buff()
    {
        return buff;
    }

    void set_as_str( TPtrC str )
    {
        buff.Zero();

        if( str.Length() < buff.MaxLength() )
                buff.Append( str );
            else
                buff.Append( str.Ptr(), buff.MaxLength()-1 );

    }

    int get_as_num()
    {
        return number;
    }

    void set_as_num( int nn )
    {
        number = nn;
    }

    TTime get_as_ttime()
    {
        return tm;
    }

    void set_as_ttime( TTime &t )
    {
        tm = t;
    }

    TReal get_as_rnumber()
    {
        return rnumber;
    }

    void set_as_rnumber( TReal r )
    {
        rnumber = r;
    }

    TInt64 get_as_bnumber()
    {
        return bnumber;
    }

    void set_as_bnumber( TInt64 b )
    {
        bnumber = b;
    }

};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

enum db_filter_item_type_e
{
    DBFT_EQUAL,
    DBFT_LESS_THAN,
    DBFT_GREATER_THAN,
    DBFT_DIFFERENT,
    DBFT_STARTS_BY,
    DBFT_ENDS_BY,
    DBFT_CONTAINS,
};


class db_filter_item_c
{
    db_data_item_c data_item;
    dbm_field_c *pfield;
    dbm_file_c *pfile;
    db_filter_item_type_e type;

    db_data_item_c stunt;

    TBuf <256> filter_str;

    public:
    db_filter_item_c(dbm_file_c *pfile);
    ~db_filter_item_c();

    db_data_item_c &get_data_item()
    {
        return data_item;
    }

    dbm_field_c *get_field()
    {
        return pfield;
    }

    db_filter_item_type_e get_type()
    {
        return type;
    }

    void start( dbm_field_c *pfield, db_data_item_c &data_item , db_filter_item_type_e type );
    bool check_on_currrent_record();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif