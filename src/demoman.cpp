/*------------------------------------------------------------------------------

                DEMONSTRATION VERSION MANAGER

    JCR 5.2004
------------------------------------------------------------------------------*/
#include "demoman.h"

#include <f32file.h>
#include <f32file.h>
#include <s32std.h>
#include <s32file.h>

_LIT( KFileName,"SYSREGV.DAT" );

#define Y_MARRABATOR (1245)
#define M_MARRABATOR (13)
#define D_MARRABATOR (35)
#define N1_MARRABATOR (39)
#define N2_MARRABATOR (54)

demo_manager_c::demo_manager_c()
{
    number1 = 0;
    number2 = 0;

    inst_date.HomeTime();
}

int demo_manager_c::get_number1()
{
    return ( number1 );
}

int demo_manager_c::get_number2()
{
    return ( number2 );
}

TTime demo_manager_c::get_inst_date()
{
    return ( inst_date );
}

int demo_manager_c::get_n_days()
{
    TTime current_date;
    current_date.HomeTime();

    TTimeIntervalDays numba = current_date.DaysFrom( inst_date );

    return numba.Int();
}

bool demo_manager_c::demo_expired( int n_valid_days )
{
    return ( get_n_days() > n_valid_days ? true : false );
}

bool demo_manager_c::save( TPtrC path )
{
    RFs fsSession;


    bool ret = false;

	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return ret;
    }

    TBuf <256> file_path( path );
    file_path.Append( KFileName );

    RFile file;
    TTime dummy;

    if( fsSession.Modified( file_path, dummy ) != KErrNone ) // File does not exist !
    {
        if( file.Create( fsSession, file_path ,EFileWrite ) != KErrNone )
            return ( ret );
    }
    else
    {
        if( file.Open( fsSession, file_path ,EFileWrite ) != KErrNone )
            return ( ret );
    }


    TBuf8 <64> des_date;

    TTime garbage;
    garbage.HomeTime();
    des_date.AppendNumFixedWidth( garbage.DateTime().MicroSecond(), EDecimal, 5 );
    des_date.AppendNumFixedWidth( inst_date.DateTime().Year() + Y_MARRABATOR, EDecimal, 4 );
    garbage.HomeTime();
    des_date.AppendNumFixedWidth( garbage.DateTime().MicroSecond(), EDecimal, 5 );
    des_date.AppendNumFixedWidth( (int)(inst_date.DateTime().Month() + M_MARRABATOR), EDecimal, 2 );
    garbage.HomeTime();
    des_date.AppendNumFixedWidth( garbage.DateTime().MicroSecond(), EDecimal, 5 );
    des_date.AppendNumFixedWidth( inst_date.DateTime().Day() + D_MARRABATOR, EDecimal, 2 );
    des_date.AppendNumFixedWidth( number1 + N1_MARRABATOR, EDecimal, 5 );
    des_date.AppendNumFixedWidth( number2 + N2_MARRABATOR, EDecimal, 5 );
    garbage.HomeTime();
    des_date.AppendNumFixedWidth( garbage.DateTime().MicroSecond(), EDecimal, 5 );

    file.Write( des_date );

    return ( true );
}

bool demo_manager_c::load( TPtrC path )
{
    RFs fsSession;
    int ret = false;
    
	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return ret;
    }

    TBuf <256> file_path( path );
    file_path.Append( KFileName );

    RFile file;
    TTime dummy;

    if( fsSession.Modified( file_path, dummy ) != KErrNone ) // File does not exist !
    {
        return ( false );
    }
    else
    {
        if( file.Open( fsSession, file_path ,EFileRead ) != KErrNone )
            return ( ret );

        TBuf8 <12> des_date_y;
        TBuf8 <12> des_date_m;
        TBuf8 <12> des_date_d;
        TBuf8 <12> dummy;
        TBuf8 <12> num1;
        TBuf8 <12> num2;

        file.Read( dummy , 5 );
        file.Read( des_date_y , 4 );
        dummy.Zero();
        file.Read( dummy , 5 );
        file.Read( des_date_m , 2 );
        dummy.Zero();
        file.Read( dummy , 5 );
        file.Read( des_date_d , 2 );
        file.Read( num1 , 5 );
        file.Read( num2 , 5 );

        int yy,mm,dd;

        TLex8 lx;
        lx.Assign( des_date_y );
        lx.Val( yy );
        lx.Assign( des_date_m );
        lx.Val( mm );
        lx.Assign( des_date_d );
        lx.Val( dd );
        lx.Assign( num1 );
        lx.Val( number1 );
        lx.Assign( num2 );
        lx.Val( number2 );


        number1 -= N1_MARRABATOR;
        number2 -= N2_MARRABATOR;

        TDateTime tdt( yy  - Y_MARRABATOR, (TMonth)(mm - M_MARRABATOR), dd - D_MARRABATOR,0,0,0,0 );
        inst_date = tdt;
    }

    file.Close();
    fsSession.Close(); //close the file session

    return ( true );
}

bool demo_manager_c::set_nums( int n1, int n2 )
{
    number1 = n1;
    number2 = n2;

    return ( true );
}
