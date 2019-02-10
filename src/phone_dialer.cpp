/*------------------------------------------------------------------------------

                        Symbian OS Phone Dialer

    JCR 6.2004
------------------------------------------------------------------------------*/
#include "phone_dialer.h"
#include <barsread.h>
#include <etel.h>
#include <eikenv.h>

_LIT (KETsyName,"phonetsy.tsy");

phone_dialer_c::phone_dialer_c()
{

}

phone_dialer_c::~phone_dialer_c()
{

}

bool phone_dialer_c::dial( const TDesC& _aPhoneNumber )
{
    // emulator does not support dialing
    // Limpa caracteres esquisitos :
    TBuf <256> aPhoneNumber;

    for( int n = 0; n < _aPhoneNumber.Length(); n++ )
    {
        TChar ch = _aPhoneNumber[n];

        if( ch.IsDigit() )
        {
            aPhoneNumber.Append( ch );
        }
    }
#ifdef __WINS__
    CEikonEnv::Static()->InfoWinL( _L("CANNOT DIAL ON THE EMMULATOR"), aPhoneNumber );
#else

    //Create a connection to the tel server
    RTelServer server;
    CleanupClosePushL(server);
    User::LeaveIfError(server.Connect());

    //Load in the phone device driver
    User::LeaveIfError(server.LoadPhoneModule(KETsyName));

    //Find the number of phones available from the tel server
    TInt numberPhones;
    User::LeaveIfError(server.EnumeratePhones(numberPhones));

    //Check there are available phones
    if (numberPhones < 1)
        {
        User::Leave(KErrNotFound);
        }

    //Get info about the first available phone
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(server.GetPhoneInfo(0, info));

    //Use this info to open a connection to the phone, the phone is identified by its name
    RPhone phone;
    CleanupClosePushL(phone);
    User::LeaveIfError(phone.Open(server, info.iName));

    //Get info about the first line from the phone
    RPhone::TLineInfo lineInfo;
    User::LeaveIfError(phone.GetLineInfo(0, lineInfo));

    //Use this to open a line
    RLine line;
    CleanupClosePushL(line);
    User::LeaveIfError(line.Open(phone, lineInfo.iName));

    //Open a new call on this line
    TBuf <100> newCallName;
    RCall call;
    CleanupClosePushL(call);
    User::LeaveIfError(call.OpenNewCall(line, newCallName));

    //newCallName will now contain the name of the call
    User::LeaveIfError(call.Dial(aPhoneNumber));

    //Close the phone, line and call connections and remove them from the cleanup stack
    //NOTE: This does not hang up the call
    CleanupStack::PopAndDestroy(3);//phone, line, call

    //Unload the phone device driver
    User::LeaveIfError(server.UnloadPhoneModule(KETsyName));

    //Close the connection to the tel server and remove it from the cleanup stack
    CleanupStack::PopAndDestroy(&server);
#endif
}
