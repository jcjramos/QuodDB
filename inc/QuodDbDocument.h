#ifndef QuodDbDOCUMENT_H
#define QuodDbDOCUMENT_H

// INCLUDES

// System includes
#include <akndoc.h> // CAknDocument

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*
* @class	CQuodDbDocument QuodDbDocument.h 
* @brief	This is the document class for a list example based on the
* standard Symbian OS architecture.  
*
* Copyright (c) EMCC Software Ltd 2003
* @version	1.0
* 
*/
class CQuodDbDocument : public CAknDocument
	{
public: // Constructor

	static CQuodDbDocument* NewL(CEikApplication& aApp);

private: // Constructors

	CQuodDbDocument(CEikApplication& aApp);
	void ConstructL();

private: // from CEikDocument

	CEikAppUi* CreateAppUiL();
	};

#endif	// #ifndef QuodDbDOCUMENT_H

// End of File

