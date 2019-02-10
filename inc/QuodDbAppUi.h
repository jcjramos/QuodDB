#ifndef QuodDbAPPUI_H
#define QuodDbAPPUI_H

// INCLUDES

// System Includes
#include <aknappui.h> // CAknAppUi
#include <aknviewappui.h>


#include "QuodDbMain.h"

// FORWARD DECLARATIONS
class CQuodDbView;
class CQuodDbAppUi;

/**
*
* @class	CQuodDbAppUi QuodDbAppUi.h
* @brief	This is the main application UI class for a list example based on the
* standard Symbian OS architecture.
*
* It provides support for the EIKON control architecture.
*
* Copyright (c) EMCC Software Ltd 2003
* @version	1.0
*
*/
class CQuodDbAppUi : public CAknViewAppUi//CAknAppUi
{
public: // Constructors and destructor

	void ConstructL();

    CQuodDbAppUi();
	~CQuodDbAppUi();

    void SetOutlineId( const TInt aOutlineId )
    {
        iCurrentOutline = aOutlineId;
    }

    TInt OutlineId() const
    {
        return iCurrentOutline;
    }


private: // from CEikAppUi

	void HandleCommandL(TInt aCommand);

private: //Data

	CQuodDbView* iAppContainer;
    TInt iCurrentOutline;

    quod_db_main_c main_app;
};


#endif	// #ifndef QuodDbAPPUI_H

// End of File
