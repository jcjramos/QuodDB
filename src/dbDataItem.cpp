/*------------------------------------------------------------------------------

                    Class DB Data Item

JCR 3.2004
------------------------------------------------------------------------------*/
#include "dbdataitem.h"
#include "dbmaster.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

db_data_item_c::db_data_item_c()
{
}

db_data_item_c::~db_data_item_c()
{
}

void db_data_item_c::reset()
{
    buff.Zero();
    number = 0;
    tm.HomeTime();
    rnumber = 0;
    bnumber= 0;

}

bool db_data_item_c::build_from_db( dbm_field_c *field , dbm_file_c *file )
{
    if( field->get_col_idx_in_table() == KDbNullColNo || file->get_table().IsColNull(field->get_col_idx_in_table()) )
    {
        reset();
        return false;
    }

    if( file->get_table().ColType( field->get_col_idx_in_table() ) != field->db_type() )
    {
        reset();
        return false;
    }

    switch( field->db_type() )
    {
        case EDbColInt32:
            number = file->get_table().ColInt( field->get_col_idx_in_table() );
            break;

        case EDbColText:
            {
                buff.Zero();
                buff.Append( file->get_table().ColDes( field->get_col_idx_in_table() ) );
            }
            break;

        case EDbColDateTime:
            {
                tm = file->get_table().ColTime( field->get_col_idx_in_table() );
            }
            break;

        case EDbColReal64:
            {

                rnumber = file->get_table().ColReal( field->get_col_idx_in_table() );
            }
            break;

        case EDbColInt64:
            bnumber = file->get_table().ColInt64( field->get_col_idx_in_table() );
            break;

        case EDbColBit:
            number = file->get_table().ColUint8( field->get_col_idx_in_table() );
            break;

        default: break;
    }

    update_str( field );
    return ( true );
}

bool db_data_item_c::update_str( dbm_field_c *field )
{
    if( field->type() ==  DFT_LIST || field->type() == DFT_YES_NO )
    {
        buff.Zero();

        if( number >= 0 && number < field->get_n_extra_descs() )
                buff.Append( field->get_extra_desc( number ) );
    }
    else
    {
        switch( field->db_type() )
        {
            case EDbColBit:
            case EDbColInt32:
                {
                    buff.Zero();
                    buff.AppendNum( number );
                }
                break;

            case EDbColText:
                break;

            case EDbColDateTime:
                {
                    if( field->type() == DFT_DATE )
                    {
                        _LIT(KDateString1,"%D%M%Y%/0%1%/1%2%/2%3%/3");
                        tm.FormatL(buff,KDateString1);
                    }
                    else
                    {
                        _LIT(KDateString5,"%-B%:0%J%:1%T%:2%S%:3%+B");
                        tm.FormatL(buff,KDateString5);
                    }
                }
                break;

            case EDbColReal64:
                {
                    buff.Zero();
                    TRealFormat ff( 12, 2 );
                    buff.AppendNum( rnumber, ff );
                }
                break;

            case EDbColInt64:
                {
                    buff.Zero();
                    buff.AppendNum( bnumber );
                }
                break;

            default: break;
        }
    }

    dbm_master_clean_tabs( buff );

    return ( true );
}

bool db_data_item_c::workout_from_str( dbm_field_c *field , TPtrC str )
{
    set_as_str( str );

    // It might contain the text instead of the value ...
    if( field->type() == DFT_LIST || field->type() == DFT_YES_NO )
    {
        TBuf <256> stunt;
        buff.UpperCase();

        for( int n = 0; n < field->get_n_extra_descs(); n++ )
        {
            stunt.Zero();
            stunt.Append( field->get_extra_desc( n ) );
            stunt.UpperCase();

            if( buff == stunt )
            {
                buff.Zero();
                buff.Append( field->get_extra_desc( n ) );
                number = n;

                return ( true );
            }
        }

    }

    switch( field->db_type() )
    {
        case EDbColBit:
        case EDbColInt32:
            {
                TLex lx( buff );

                if( lx.Val( number ) != KErrNone  )
                        number = 0;

            }
            break;

        case EDbColText:
            break;

        case EDbColDateTime:
            {
                tm.Parse( buff );
            }
            break;

        case EDbColReal64:
            {
                TLex lx( buff );

                if( lx.Val( rnumber ) != KErrNone  )
                        rnumber = 0;

            }
            break;

        case EDbColInt64:
            {
                TLex lx( buff );

                if( lx.Val( bnumber ) != KErrNone  )
                        bnumber = 0;

            }
            break;

        default: break;
    }

    return ( true );
}

bool db_data_item_c::to_db( dbm_field_c *field , dbm_file_c *file  )
{
    if( KDbNullColNo == field->get_col_idx_in_table() )
            return ( false );

    if( file->get_table().ColType( field->get_col_idx_in_table() ) != field->db_type() )
    {
        return false;
    }

    switch( field->db_type() )
    {
        case EDbColBit:
        case EDbColInt32:
            file->get_table().SetColL( field->get_col_idx_in_table() , number );
            break;

        case EDbColText:
            {
                if( buff.Length() >= field->max_len() )
                {
                    buff.SetLength( field->max_len()-1 );
                }

                file->get_table().SetColL( field->get_col_idx_in_table() , buff );
            }
            break;

        case EDbColDateTime:
            file->get_table().SetColL( field->get_col_idx_in_table() , tm );
            break;

        case EDbColReal64:
            file->get_table().SetColL( field->get_col_idx_in_table() , rnumber );
            break;

        case EDbColInt64:
            file->get_table().SetColL( field->get_col_idx_in_table() , bnumber );
            break;
        default: break;
    }
    return ( true );

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define GENERIC_COMPARE( x , y )\
{\
    if( type == DBFT_EQUAL && x == y )\
            return ( true );\
    if( type == DBFT_LESS_THAN && x < y )\
            return ( true );\
    if( type == DBFT_GREATER_THAN && x > y )\
            return ( true );\
    if( type == DBFT_DIFFERENT && x != y )\
            return ( true );\
}

db_filter_item_c::db_filter_item_c( dbm_file_c *_pfile )
{
    pfile = _pfile;
}

db_filter_item_c::~db_filter_item_c()
{

}

void db_filter_item_c::start( dbm_field_c *_pfield, db_data_item_c &_data_item , db_filter_item_type_e _type )
{
    pfield = _pfield;
    data_item = _data_item;
    type = _type;
}

bool db_filter_item_c::check_on_currrent_record()
{
    stunt.build_from_db( pfield , pfile );

    switch( pfield->db_type() )
    {
        case EDbColBit:
        case EDbColInt32:
            {
                GENERIC_COMPARE( stunt.get_as_num() , data_item.get_as_num() );
            }
            break;

        case EDbColText:
            {
                filter_str.Zero();
                filter_str.Append( data_item.get_as_str() );
                filter_str.UpperCase();

                stunt.get_as_buff().UpperCase();

                GENERIC_COMPARE( stunt.get_as_str() , filter_str );

                switch( type )
                {
                    case DBFT_STARTS_BY:
                            {
                                TPtrC left = stunt.get_as_str().Left( filter_str.Length() );
                                if( left == filter_str )
                                        return ( true );
                            }
                            break;

                    case DBFT_ENDS_BY:
                            {
                                TPtrC right = stunt.get_as_str().Right( filter_str.Length() );
                                if( right == filter_str )
                                    return ( true );
                                break;
                            }

                    case DBFT_CONTAINS:
                            {
                                int ret = stunt.get_as_str().Find( filter_str );
                                if( ret != KErrNotFound )
                                        return ( true );
                                break;
                            }
                    default: break;

                }
            }
            break;

        case EDbColDateTime:
            {
                GENERIC_COMPARE( stunt.get_as_ttime(), data_item.get_as_ttime() );
            }
            break;

        case EDbColReal64:
            {
                GENERIC_COMPARE( stunt.get_as_rnumber() , data_item.get_as_rnumber() );
            }
            break;

        case EDbColInt64:
            {
                GENERIC_COMPARE( stunt.get_as_bnumber() , data_item.get_as_bnumber() );
            }
            break;
        default: break;

    }

    return ( false );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

