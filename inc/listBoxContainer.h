/*------------------------------------------------------------------------------

                      List Box Container
                Generic ListBox Control Container

    JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef LISTBOXCONTAINER_H
#define LISTBOXCONTAINER_H

// INCLUDES
#include <coecntrl.h>				// CCoeControl
#include <eiklbo.h>					// MEikListBoxObserver
#include <eiklbx.h>
#include <eiklbi.h>
#include <aknlists.h>				// CAknSingleNumberStyleListBox
#include <eikclbd.h>
#include <AknQueryDialog.h>			// CAknQueryDialog


enum list_box_container_types_e
{
    LBCT_NO_HEADER,
    LBCT_HEADER,
    LBCT_NUMERIC_HEADER
};

// FORWARD DECLARATIONS
class CEikColumnListBox;
class CEikListBox;

// INherit from this guy to know stuff about the ListBox
class list_box_container_enhanced_observer_c
{
public:
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType) = 0;
    virtual void HandleListBoxEventL( CEikListBox* aListBox, MEikListBoxObserver::TListBoxEvent aListBoxEvent ) = 0;
};

// CLASS DECLARATION
class list_box_container_c
		: public MEikListBoxObserver,
		  public CCoeControl
	{
public: // constructors and destructor
	static list_box_container_c* NewL(const TRect& aRect , list_box_container_enhanced_observer_c *observer = 0 );
	static list_box_container_c* NewLC(const TRect& aRect , list_box_container_enhanced_observer_c *observer = 0);

    list_box_container_c( list_box_container_enhanced_observer_c *observer = 0);
	~list_box_container_c();

	void ConstructL(const TRect& aRect);

    void Rebuild( list_box_container_types_e type , CDesCArrayFlat* data = 0, TListBoxModelItemArrayOwnership own_type = ELbmOwnsItemArray /*OR:ELbmDoesNotOwnItemArray*/ , bool redraw = true );

    int GetCurrentItemIdx();
    int GetNumListItems();

    void SetCurrentItemIdx( int item );

    void SetTopItemIdx( int item );
    int GetTopItemIdx();

    void Redraw();

private: // from MEikListBoxObserver
	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aListBoxEvent);

private: // from CoeControl
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void Draw(const TRect& aRect) const;
	void SizeChanged();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

private: // data
	CEikColumnListBox*				iListBox;
    list_box_container_enhanced_observer_c *observer;
	};


#endif

