/*------------------------------------------------------------------------------

                      QUodDB Container
                ListBox Control Container

    JCR 3.2004
------------------------------------------------------------------------------*/

//  Class include
#include "ListBoxContainer.h"



#define KAknExListGray TRgb(0xaaaaaa)

list_box_container_c* list_box_container_c::NewL(const TRect& aRect , list_box_container_enhanced_observer_c *observer )
{
	list_box_container_c* self = list_box_container_c::NewLC(aRect , observer );
	CleanupStack::Pop(self);
	return self;
}

list_box_container_c* list_box_container_c::NewLC(const TRect& aRect, list_box_container_enhanced_observer_c *observer  )
{
	list_box_container_c* self = new (ELeave) list_box_container_c( observer );
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
}


list_box_container_c::list_box_container_c( list_box_container_enhanced_observer_c *_observer )
{
    observer = _observer;
    iListBox = 0;
}

list_box_container_c::~list_box_container_c()
{
	delete iListBox;
}

void list_box_container_c::ConstructL(const TRect& aRect)
{
	CreateWindowL();

	iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
	iListBox->ConstructL(this, EAknListBoxSelectionList);
	iListBox->SetContainerWindowL(*this);
	iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	iListBox->SetRect(Rect() );
	iListBox->SetListBoxObserver(this);

    /*
    /// Show the empty list box text rather than the default "no data"
	HBufC* emptyListText = iEikonEnv->AllocReadResourceLC(R_EMPTY_LIST_TEXT);
	iListBox->View()->SetListEmptyTextL(*emptyListText);
	CleanupStack::PopAndDestroy (emptyListText);
    */

	SetRect(aRect);
	ActivateL();
}

void list_box_container_c::SizeChanged()
{
	if (iListBox)
	{
		iListBox->SetRect(Rect());
	}
}

TInt list_box_container_c::CountComponentControls() const
{
	return 1;
}

CCoeControl* list_box_container_c::ComponentControl(TInt aIndex) const
{
	switch (aIndex)
	{
		case 0:
			return iListBox;
		default:
			return NULL;
	}
}


/**
* Called by the framework to draw this control.  Clears the area in
* aRect.
* @param aRect in which to draw
*/
void list_box_container_c::Draw(const TRect& aRect) const
{
    // (Re)draw the rectangle of frame.
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KAknExListGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
}


TKeyResponse list_box_container_c::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if( observer )
    {
        observer->OfferKeyEventL( aKeyEvent, aType);
    }

    if (iListBox)
    {
		return iListBox->OfferKeyEventL(aKeyEvent, aType);
    }

	return EKeyWasNotConsumed;
}

void list_box_container_c::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aListBoxEvent)
{
    if( observer )
    {
        observer->HandleListBoxEventL( aListBox, aListBoxEvent );
    }
}

int list_box_container_c::GetCurrentItemIdx()
{
    if( iListBox )
    {
        return iListBox->CurrentItemIndex();
    }

    return (-1);
}

int list_box_container_c::GetNumListItems()
{
    if( iListBox )
    {
        return iListBox->Model()->NumberOfItems();
    }

    return ( 0 );
}

void list_box_container_c::SetCurrentItemIdx( int item )
{
    if( iListBox )
    {
        iListBox->SetCurrentItemIndex( item );
        iListBox->DrawNow();
    }
}

void list_box_container_c::SetTopItemIdx( int item )
{
    if( iListBox )
        iListBox->SetTopItemIndex( item );
}

int list_box_container_c::GetTopItemIdx()
{
    if( iListBox )
    {
        return iListBox->TopItemIndex();
    }

    return ( 0 );
}


void list_box_container_c::Rebuild( list_box_container_types_e type , CDesCArrayFlat* data , TListBoxModelItemArrayOwnership own_type , bool redraw )
{
    delete iListBox;
    iListBox = NULL;

    switch( type )
    {
        case LBCT_NO_HEADER:
             iListBox = new (ELeave) CAknSingleStyleListBox();
             break;

        case LBCT_HEADER:
             iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
             break;

        case LBCT_NUMERIC_HEADER:
            //iListBox = new (ELeave) CAknSingleNumberHeadingStyleListBox();
            iListBox = new (ELeave) CAknSingleNumberStyleListBox();
            break;
    }


    iListBox->ConstructL(this, EAknListBoxSelectionList);
    iListBox->SetContainerWindowL(*this);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iListBox->SetRect(Rect() );
    iListBox->SetListBoxObserver(this);

    if( data )
    {
    	CTextListBoxModel* model = iListBox->Model();

	    // Set the model data
	    model->SetItemTextArray( data );
    	model->SetOwnershipType( own_type );

        iListBox->HandleItemAdditionL();
    }

    if( redraw )
    {
        iListBox->ActivateL();
        DrawNow();
    }
}

void list_box_container_c::Redraw()
{
    if( iListBox )
        iListBox->ActivateL();
        
    DrawNow();

}
// End of File
