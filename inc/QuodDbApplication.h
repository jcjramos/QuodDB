#ifndef QuodDbAPPLICATION_H
#define QuodDbAPPLICATION_H

// INCLUDES
// System includes
#include <aknapp.h> // CAknApplication

// CONSTANTS
// UID of the application
#ifdef __OS91__
const TUid KUidQuodDb = {0xA00011DF};
#else
const TUid KUidQuodDb = {0x102010FB};
#endif
// CLASS DECLARATION

/**
*
* @class	CQuodDbApplication QuodDbApplication.h 
* @brief	This is the main application class for a list example based on the
* standard Symbian OS architecture.  
*
* It provides a factory to create a concrete document object, and a method to retrieve
* the application's UID.
*
* Copyright (c) EMCC Software Ltd 2003
* @version	1.0
* 
*/
class CQuodDbApplication : public CAknApplication
{
private:  // from CApaApplication
	
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
};

#endif	// #ifndef QuodDbAPPLICATION_H

// End of File

