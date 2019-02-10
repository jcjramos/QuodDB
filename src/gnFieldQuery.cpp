/*------------------------------------------------------------------------------

                    Generic Field Query

JCR 3.2004
------------------------------------------------------------------------------*/
#include "gnFieldQuery.h"

#include <aknquerydialog.h>

#include "genAppRes.h"

#ifndef __GEN_APP_RESOURCE_INCLUDE_FILE__
#error  Please create a GenAppRes.H file including the AppName.RSH & RSH files
#error  Then include dataView.RSS in your application resource file! 
#endif


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
gn_field_query_c::gn_field_query_c()
{

}

gn_field_query_c::~gn_field_query_c()
{

}

bool gn_field_query_c::query_field(  dbm_field_c *ff , TPtrC textData )
{
    bool ret = false;

    switch( ff->type() )
    {
        case DFT_INT:
            {
              int number;
              CAknNumberQueryDialog* dlg = new(ELeave) CAknNumberQueryDialog( number, CAknQueryDialog::ENoTone );
              if (dlg->ExecuteLD( R_GNFIELDQUERY_NUMBER_LAYOUT , textData ))
              {
                    data_item.set_as_num( number );
                    ret = true;
              }

            } break;

        case DFT_TEXT:
            {

              CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog( buff, CAknQueryDialog::ENoTone );
              if (dlg->ExecuteLD( R_GNFIELDQUERY_DATA_QUERY , textData ))
              {
                    ret = true;
                    data_item.set_as_str( buff );
              }

            } break;

        case DFT_DATE:
            {
                TTime tm;
                tm.HomeTime();

                //CAknTimeQueryDialog* dlg = new(ELeave) CAknTimeQueryDialog( tm, CAknQueryDialog::ENoTone );

                CAknTimeQueryDialog* dlg = CAknTimeQueryDialog::NewL( tm, CAknQueryDialog::ENoTone );

                if (dlg->ExecuteLD( R_GNFIELDQUERY_DATE_QUERY_EX , textData ))
                {
                      data_item.set_as_ttime( tm );
                      ret = true;
                }
            }break;

        case DFT_TIME:
            {
                TTime tm;
                CAknTimeQueryDialog* dlg = new(ELeave) CAknTimeQueryDialog( tm, CAknQueryDialog::ENoTone );
                if (dlg->ExecuteLD( R_GNFIELDQUERY_TIME_QUERY , textData ))
                {
                    data_item.set_as_ttime( tm );
                    ret = true;
                }
            }break;

        case DFT_FLOAT:
            {
              TReal rnumber;
             CAknFloatingPointQueryDialog* dlg = new(ELeave) CAknFloatingPointQueryDialog( rnumber, CAknQueryDialog::ENoTone );
              if (dlg->ExecuteLD( R_GNFIELDQUERY_BIGNUMBER_QUERY , textData ))
              {
                    data_item.set_as_rnumber( rnumber );
                    ret = true;
              }
            }

        case DFT_TEXT_NUMBER:
        case DFT_PHONE_NUMBER:
        case DFT_BIGNUMBER:
            {
              CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog( buff, CAknQueryDialog::ENoTone );
              if (dlg->ExecuteLD( R_GNFIELDQUERY_BIGNUMBER_QUERY , textData ))
              {
                    if( ff->type() == DFT_PHONE_NUMBER || ff->type() == DFT_TEXT_NUMBER )
                    {
                        data_item.set_as_str( buff );
                    }
                    else
                    {
                        TInt64 bnumber;
                        buff.Num( bnumber );

                        data_item.set_as_bnumber( bnumber );
                    }

                    ret = true;
              }
            }
            break;

        case DFT_LIST:
        case DFT_YES_NO:
            {
                  if( !ff->get_n_extra_descs() )
                        return false;

                  CDesCArrayFlat *tArray = new (ELeave) CDesCArrayFlat(10);

                  for( int n = 0; n < ff->get_n_extra_descs(); n++ )
                  {
                          tArray->AppendL( ff->get_extra_desc( n ) );
                  }

                  TInt index( 0 );

                  CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );

                  dlg->PrepareLC( R_GNFIELDQUERY_LIST_SINGLESEL );
                  dlg->SetOwnershipType( ELbmOwnsItemArray );
                  dlg->SetItemTextArray( tArray );

                  dlg->SetHeaderTextL( textData );
                  TInt answer = dlg->RunLD();

                  if( answer )
                  {
                        ret = true;

                        data_item.set_as_num( index );
                  }

            }break;

        default: break;

    }

    if( ret == true )
    {
        data_item.update_str( ff );
    }

    return ( ret );
}

