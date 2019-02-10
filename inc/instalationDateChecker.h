/*------------------------------------------------------------------------------

                Instalation Date Checker

    JCR 4.2004
------------------------------------------------------------------------------*/
#ifndef INSTALATION_DATE_CHECKER_H
#define INSTALATION_DATE_CHECKER_H

#include <e32std.h>

class instalation_date_checker_c
{
  public:
    instalation_date_checker_c();

    TTime check_instalation_date( TPtrC file_name );
};


#endif