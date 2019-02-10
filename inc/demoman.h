/*------------------------------------------------------------------------------

                DEMONSTRATION VERSION MANAGER

    JCR 5.2004
------------------------------------------------------------------------------*/
#ifndef _DEMOMAN_H_
#define _DEMOMAN_H_

#include <e32std.h>

class demo_manager_c
{
    int number1;
    int number2;

    TTime inst_date;

  public:
    demo_manager_c();

    bool load( TPtrC path );
    bool save( TPtrC path );

    int get_number1();
    int get_number2();
    TTime get_inst_date();

    int get_n_days();

    bool demo_expired( int n_valid_days );

    bool set_nums( int n1, int n2 );
};


#endif
