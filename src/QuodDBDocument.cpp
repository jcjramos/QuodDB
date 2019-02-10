/**

* 
* @brief Definition of CQuodDbDocument
*
* Copyright (c) EMCC Software Ltd 2003
* @version 1.0
*/

// INCLUDE FILES

// Class include
#include "QuodDbDocument.h"

// User includes
#include "QuodDbAppUi.h"	// CQuodDbAppUi

// ================= MEMBER FUNCTIONS =======================

/**
* Constructs the document for aApp
*
* @param aApp for which the document is constructed
*/
CQuodDbDocument::CQuodDbDocument(CEikApplication& aApp)
: CAknDocument(aApp)	
	{
	}

/**
* Symbian OS 2nd phase constructor.
* This is an empty implementation.
*/
void CQuodDbDocument::ConstructL()
	{
	}

/**
* Creates the AppUi for this document.
* @return The newly constructed AppUi
*/	 
CEikAppUi* CQuodDbDocument::CreateAppUiL()
	{
	return new (ELeave) CQuodDbAppUi;
	}

/**
* Symbian OS 2 phase constructor.
* Constructs the CQuodDbDocument using the constructor and the ConstructL 
* method.
* @param aApp for which the CQuodDbDocument is constructed
* @return The newly constructed CQuodDbDocument
*/
CQuodDbDocument* CQuodDbDocument::NewL(CEikApplication& aApp) 	
	{
	CQuodDbDocument* self = new (ELeave) CQuodDbDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// End of File	
