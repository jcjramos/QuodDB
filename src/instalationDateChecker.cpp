/*------------------------------------------------------------------------------

                Instalation Date Checker

    JCR 4.2004
------------------------------------------------------------------------------*/
#include "instalationDateChecker.h"


#include <f32file.h>
#include <f32file.h>
#include <s32std.h>
#include <s32file.h>


instalation_date_checker_c::instalation_date_checker_c()
{

}

TTime instalation_date_checker_c::check_instalation_date( TPtrC file_name )
{
    TTime ret;
    ret.HomeTime();

    RFs fsSession;

	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return ret;
    }

    _LIT( pataa, "C:\\SYSTEM\\DATA\\" );
    TBuf <256> file_path( pataa );
    file_path.Append( file_name );

    TTime dummy;

    RFile file;


    if( fsSession.Modified( file_path, dummy ) != KErrNone ) // File does not exist !
    {
        if( file.Create( fsSession, file_path ,EFileRead ) != KErrNone )
            return ( ret );


        TBuf8 <12> des_date;
        des_date.AppendNumFixedWidth( ret.DateTime().Year(), EDecimal, 4 );
        des_date.AppendNumFixedWidth( (int)(ret.DateTime().Month() + 1), EDecimal, 2 );
        des_date.AppendNumFixedWidth( ret.DateTime().Day(), EDecimal, 2 );

        file.Write( des_date );
    }
    else
    {
        if( file.Open( fsSession, file_path ,EFileRead ) != KErrNone )
            return ( ret );

        TBuf8 <12> des_date_y;
        TBuf8 <12> des_date_m;
        TBuf8 <12> des_date_d;

        file.Read( des_date_y , 4 );
        file.Read( des_date_m , 2 );
        file.Read( des_date_d , 2 );

        int yy,mm,dd;

        TLex8 lx;
        lx.Assign( des_date_y );
        lx.Val( yy );
        lx.Assign( des_date_m );
        lx.Val( mm );
        lx.Assign( des_date_d );
        lx.Val( dd );


        TDateTime tdt( yy , (TMonth)(mm - 1), dd,0,0,0,0 );
        ret = tdt;
    }

    file.Close();
    fsSession.Close(); //close the file session

    return ret;
}

