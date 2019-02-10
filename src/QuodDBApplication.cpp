/**
* 
* @brief Definition of CListDlgApplication
*
* Copyright (c) EMCC Software Ltd 2003
* @version 1.0
*/

// INCLUDE FILES

// Class include
#include "QuodDbApplication.h"

// User includes
#include "QuodDbDocument.h" // CListDlgDocument

#ifdef __OS91__
#include <eikstart.h>
#endif
// ================= MEMBER FUNCTIONS =======================

/**
* @brief From CApaApplication, returns the application's UID (KUidListDlg).
* @return The value of KUidListDlg.
*/
TUid CQuodDbApplication::AppDllUid() const
	{
	return KUidQuodDb;
	}


/**
* @brief From CApaApplication, creates a CListDlgDocument object.
* @return A pointer to the created document object.
*/
CApaDocument* CQuodDbApplication::CreateDocumentL()
	{
	return CQuodDbDocument::NewL(*this);
	}

// ================= OTHER EXPORTED FUNCTIONS ==============

/**
* Constructs a CListDlgApplication.
* @return A pointer to the application object.
*/
EXPORT_C CApaApplication* NewApplication()
	{
	return new CQuodDbApplication;
	}

/**
* @brief This is the entry point function for this Series 60 App
* @return KErrNone No error
*/
#ifdef __OS91__
GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication( NewApplication );
	
}
#else
GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}

#endif
// End of File

