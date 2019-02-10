/**
*
* @brief Definition of CQuodDbAppUi
*
* Copyright (c) EMCC Software Ltd 2003
* @version 1.0
*/

// INCLUDE FILES

// Class include
#include "QuodDbAppUi.h"

// System includes
#include <eikmenup.h> // CEikMenuPane
#include <aknapp.h> 

// User includes
#include "QuodDbView.h" // CQuodDbContainer
#include "QuodDbViewMain.h" // CQuodDbContainer


//#include "QuodDb.rsg"
#include "genAppRes.h"

#include "dataView.h"

// ================= MEMBER FUNCTIONS =======================

/**
* Symbian OS 2nd phase constructor.  Constructs the application's container,
* setting itself as the container's MOP parent, and adds it to the control
* stack.
*/
void CQuodDbAppUi::ConstructL()
	{
    iCurrentOutline = 0;
    	
#ifdef __OS91__
	BaseConstructL(EAknEnableSkin );//Form UI component assumes skin support KIS000292

	RFs   fsSession;
	TRAPD(err, fsSession.Connect(); );
    if (err)
    {
    	User::Leave(err);
	}
	CleanupClosePushL(fsSession);

	TBuf<KMaxFileName+KMaxPath>	privatePathAndFilename;
	fsSession.PrivatePath(privatePathAndFilename);

	#ifdef __WINS__
		privatePathAndFilename.Insert(0,_L("c:"));
	#else
		privatePathAndFilename.Insert(0,
			CEikonEnv::Static()->EikAppUi()->Application()->AppFullName().Left(2)
			);
	#endif
	//Create the folder
	fsSession.MkDirAll(privatePathAndFilename);
	CleanupStack::PopAndDestroy(&fsSession);
#else
	BaseConstructL();
#endif	

    main_app.init();

/* Was ... with controls
	iAppContainer = CQuodDbContainer::NewL(ClientRect());
	iAppContainer->SetMopParent(this);
	AddToStackL(iAppContainer);
*/

    CQuodDbViewMain* mainView = new( ELeave ) CQuodDbViewMain( &main_app );
    CleanupStack::PushL( mainView );
    mainView->ConstructL();
    AddViewL( mainView ); // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();

    CQuodDbView* dataDef = new( ELeave ) CQuodDbView( &main_app );
    CleanupStack::PushL( dataDef );
    dataDef->ConstructL();
    AddViewL( dataDef ); // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();

    data_view_c *dataV = new (ELeave) data_view_c( main_app.get_data_view_handler() );
    CleanupStack::PushL( dataV );
    dataV->ConstructL();
    AddViewL( dataV );
    CleanupStack::Pop();

    main_app.set_main_view( mainView->Id() );
    main_app.set_data_view( dataV->Id() );
    main_app.set_data_def_view( dataDef->Id() );

    ActivateLocalViewL( mainView->Id() ); // Activates column view.
}

/**
* Destructor.
* Removes the application's container from the stack and deletes it.
*/
CQuodDbAppUi::CQuodDbAppUi():main_app( this )
{

}

CQuodDbAppUi::~CQuodDbAppUi()
	{
/*
	if (iAppContainer)
		{
		RemoveFromStack(iAppContainer);
		delete iAppContainer;
		}
*/
	}

/**
* From CEikAppUi, takes care of command handling.
*
* @param aCommand command to be handled
*/
void CQuodDbAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EQuodDbCmdPlayGame:
			{
			// play the currently selected saved game
			break;
			}
		case EAknSoftkeyBack:
            {
            }
            break;
            
        case EAknSoftkeyExit:
		case EEikCmdExit:
			{
            main_app.will_exit();

			Exit();
			break;
			}
		default:
			break;		
		}
	}

// End of File	
