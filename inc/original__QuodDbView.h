#ifndef QuodDbVIEW_H
#define QuodDbVIEW_H

// INCLUDES

// System includes
#include <coecntrl.h> // CCoeControl
#include <eiklbo.h> // MEikListBoxObserver
#include <aknview.h>
#include <aknviewappui.h>
#include <aknnavi.h>
#include <akntabgrp.h>

// FORWARD DECLARATIONS
class CAknColumnListBox;
class CQuodDbContainer;

// CLASS DECLARATION

/**
*
* @class	CQuodDbContainer QuodDbContainer.h 
* @brief	This is the container class for a list example based on the
* standard Symbian OS architecture.  
*
* Copyright (c) EMCC Software Ltd 2003
* @version	1.0
* 
*/
class CQuodDbView : public CAknView//CCoeControl
	{
public: // Constructors and destructor
    CQuodDbView();
    virtual ~CQuodDbView();

	static CQuodDbView* NewL(const TRect& aRect);
	static CQuodDbView* NewLC(const TRect& aRect);


	void ConstructL(const TRect& aRect);

    TUid Id() const
    {
        TUid KAknExListColumnViewId = { 100 };
        return KAknExListColumnViewId;
    }

    void DoActivateL  (  const TVwsViewId &    aPrevViewId,TUid    aCustomMessageId, const TDesC8 &    aCustomMessage );
    void DoDeactivate  (     ) ;
    void HandleCommandL  (  TInt    aCommand  );


    void DisplayInitOutlineL();
    void SetNavigationPaneL();
    virtual void SetTitlePaneL( const TInt aOutlineId );

private: // from CoeControl

	void Draw(const TRect& aRect) const;

private: // members

	void PlaySelectedGame(TInt aOpenedItem);

    CQuodDbContainer *iContainer;
    CAknNavigationControlContainer* iNaviPane;
    CAknNavigationDecorator* iDecoratedTabGroup;
    CAknTabGroup* iTabGroup;
};

#endif	// #ifndef QuodDbCONTAINER_H

// End of File
 