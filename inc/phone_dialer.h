/*------------------------------------------------------------------------------

                        Symbian OS Phone Dialer

    JCR 6.2004
------------------------------------------------------------------------------*/
#ifndef PHONE_DIALER_H
#define PHONE_DIALER_H

#include <e32std.h>

class phone_dialer_c
{
public:
    phone_dialer_c();
    ~phone_dialer_c();

    bool dial( const TDesC& _aPhoneNumber );
};

#endif