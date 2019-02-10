/*------------------------------------------------------------------------------

                      QUodDB Container
                ListBox Control Container

    JCR 3.2004
------------------------------------------------------------------------------*/

#ifndef QUODDBCONTAINER_H
#define QUODDBCONTAINER_H

// INCLUDES
#include <coecntrl.h>				// CCoeControl
#include <eiklbo.h>					// MEikListBoxObserver


// FORWARD DECLARATIONS
class CEikColumnListBox;
class CQuodDbView;
// CLASS DECLARATION

class CQuodDbContainer
		: public MEikListBoxObserver,
		  public CCoeControl
	{
public: // constructors and destructor
	static CQuodDbContainer* NewL(const TRect& aRect , CQuodDbView * view );
	static CQuodDbContainer* NewLC(const TRect& aRect , CQuodDbView * view);

    CQuodDbContainer( CQuodDbView * view );
	~CQuodDbContainer();

	void ConstructL(const TRect& aRect);

    void Update();

    int GetCurrentItemIdx();

private: // from MEikListBoxObserver
	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aListBoxEvent);

private: // from CoeControl
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void Draw(const TRect& aRect) const;
	void SizeChanged();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

private: // new methods
	void SetListBoxArray();

private: // data
	CEikColumnListBox*				iListBox;
    CQuodDbView * view;
	};

#endif	// __ALARMORGANISER_DEFAULT_CONTAINER_H__
