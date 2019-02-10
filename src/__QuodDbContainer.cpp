/*------------------------------------------------------------------------------

                      QUodDB Container
                ListBox Control Container

    JCR 3.2004
------------------------------------------------------------------------------*/

//  Class include
#include "QuodDbContainer.h"
#include "QuodDbView.h"
#include "DbMaster.h"

#include <aknlists.h>				// CAknSingleNumberStyleListBox
#include <eiklbi.h>
#include <eikclbd.h>
#include <AknQueryDialog.h>			// CAknQueryDialog

#include <quoddb.rsg>

#define KAknExListGray TRgb(0xaaaaaa)

CQuodDbContainer* CQuodDbContainer::NewL(const TRect& aRect , CQuodDbView * view)
{
	CQuodDbContainer* self = CQuodDbContainer::NewLC(aRect , view );
	CleanupStack::Pop(self);
	return self;
}

CQuodDbContainer* CQuodDbContainer::NewLC(const TRect& aRect, CQuodDbView * view)
{
	CQuodDbContainer* self = new (ELeave) CQuodDbContainer( view );
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
}


CQuodDbContainer::CQuodDbContainer( CQuodDbView * _view )
{
    view = _view;
    iListBox = 0;
}

CQuodDbContainer::~CQuodDbContainer()
{
	delete iListBox;
}

void CQuodDbContainer::ConstructL(const TRect& aRect)
{
	CreateWindowL();

	iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
	iListBox->ConstructL(this, EAknListBoxSelectionList);
	iListBox->SetContainerWindowL(*this);
	iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	iListBox->SetRect(Rect() );
	iListBox->SetListBoxObserver(this);

    /// Show the empty list box text rather than the default "no data"
	HBufC* emptyListText = iEikonEnv->AllocReadResourceLC(R_EMPTY_LIST_TEXT);
	iListBox->View()->SetListEmptyTextL(*emptyListText);
	CleanupStack::PopAndDestroy (emptyListText);

	SetRect(aRect);
	ActivateL();
}
/*
void CQuodDbContainer::ConstructL(const TRect& aRect)
{
	CreateWindowL();

	// Create the List Box
	iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
	iListBox->ConstructL(this, EAknListBoxSelectionList);
	iListBox->SetContainerWindowL(*this);
	iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	iListBox->SetRect(Rect() );
	iListBox->SetListBoxObserver(this);
*/
//    CColumnListBoxItemDrawer* drawer = iListBox->ItemDrawer();
//    CColumnListBoxData* colData = drawer->ColumnData();
//	iListBox->CreateScrollBarFrameL(ETrue);
//    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);



/*
	SetRect(aRect);
	ActivateL();
*/
/*
    // ADDED
    CDesCArrayFlat* tArray = new (ELeave) CDesCArrayFlat(1);
    tArray->AppendL(_L("Hello\txxxxxx"));
    tArray->AppendL(_L("2\txxxxxx"));
    tArray->AppendL(_L("3\txxxxxx"));
    tArray->AppendL(_L("4\txxxxxx"));
    tArray->AppendL(_L("5\txxxxxx"));
    tArray->AppendL(_L("6\txxxxxx"));
    tArray->AppendL(_L("7\txxxxxx"));
    tArray->AppendL(_L("8\txxxxxxx"));
    tArray->AppendL(_L("9\txxxxxx"));
    tArray->AppendL(_L("10\txxxxx"));

    iListBox->Model()->SetItemTextArray(tArray);
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    iListBox->HandleItemAdditionL();

//    colData->SetColumnWidthPixelL(0,50);
//    colData->SetColumnFontL(0,LatinPlain12());
//    iListBox->UpdateScrollBarsL();
*/
/*}*/

void CQuodDbContainer::SizeChanged()
{
	if (iListBox)
	{
		iListBox->SetRect(Rect());
	}
}

TInt CQuodDbContainer::CountComponentControls() const
{
	return 1;
}

CCoeControl* CQuodDbContainer::ComponentControl(TInt aIndex) const
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
void CQuodDbContainer::Draw(const TRect& aRect) const
{
    // (Re)draw the rectangle of frame.
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KAknExListGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
}


/**
* Called by the framework whenever a key event occurs.
* Passes the key event to the listbox if it is not null, otherwise returns
* EKeyWasNotConsumed
* @param aKeyEvent the Key event which occured, e.g. select key pressed
* @param aType the type of Key event which occurred, e.g. key up, key down
* @return TKeyResponse EKeyWasNotConsumed if the key was not processed, EKeyWasConsumed if it was
*/
TKeyResponse CQuodDbContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	TChar charCode(aKeyEvent.iCode);

    switch (charCode)
		{
		// Switches tab
        case EKeyLeftArrow: // Left key.
        case EKeyRightArrow: // Right Key.
			break;
		default:
			{
			if (iListBox)
				{
				return iListBox->OfferKeyEventL(aKeyEvent, aType);
				}
			}
		}

	return EKeyWasNotConsumed;
	}

/**
* Handles listbox event. Calls remove alarm
* @param aListBox Pointer to ListBox object is not used.
* @param aEventType Type of listbox event.
*/
void CQuodDbContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aListBoxEvent)
	{
	// if the Select Key has been pressed
	if ((aListBoxEvent == MEikListBoxObserver::EEventEnterKeyPressed) ||
		(aListBoxEvent == MEikListBoxObserver::EEventItemClicked))
		{

		}
	}

/**
* This functions sets the data displayed in the list box
* to the array created by CAlarmOrganiserDefaultEngine
*/
void CQuodDbContainer::SetListBoxArray()
{
/*
	CTextListBoxModel* model = iListBox->Model();

	// Set the model data
	model->SetItemTextArray(iEngine->ListBoxArray());
	model->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iListBox->Reset();
    DrawNow();
*/

    TBuf <256> stunt;
    CDesCArrayFlat* tArray = 0;

    switch( view->MainApp()->get_mode() )
    {
        case QVM_DATABASE_LIST:
        {
            quod_db_master_c *dbm = view->MainApp()->get_db_master();
            if( !dbm->n_databases() )
                    return;

            tArray = new (ELeave) CDesCArrayFlat(10);

            for( int n = 0; n < dbm->n_databases(); n++ )
            {
                stunt.Zero();

                TChar tab('\t');
                stunt.Append( tab );
                stunt.Append( dbm->database_name( n ) );

                tArray->AppendL( stunt );
            }

        }
        break;

        case QVM_TABLE_LIST:
        {
            quod_db_master_c *dbm = view->MainApp()->get_db_master();

            if( dbm->get_current_db() && dbm->get_current_db()->n_files() )
            {
                dbm_master_c *database = dbm->get_current_db();

                tArray = new (ELeave) CDesCArrayFlat(10);

                for( int n = 0; n < database->n_files(); n++ )
                {
                    stunt.Zero();

                    TChar tab('\t');
                    stunt.Append( tab );
                    stunt.Append( database->get_file(n)->name() );

                    tArray->AppendL( stunt );
                }
            }
        }
        break;

        case QVM_TABLE:
        {
            tArray = new (ELeave) CDesCArrayFlat(10);
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));
            tArray->AppendL(_L("Ola \txxxxxx"));
            tArray->AppendL(_L("Ola \txxfjsdjfs"));

        }
        break;
    }

    if( tArray )
    {
        iListBox->Model()->SetItemTextArray(tArray);
        iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    }

}

void CQuodDbContainer::Update()
{
    delete iListBox;
    iListBox = NULL;

    if( view->MainApp()->get_mode() == QVM_DATABASE_LIST || view->MainApp()->get_mode() == QVM_TABLE_LIST )
    {
        iListBox = new (ELeave) CAknSingleStyleListBox();
    }
    else //QVM_TABLE
    {
        iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
    }

    iListBox->ConstructL(this, EAknListBoxSelectionList);
    iListBox->SetContainerWindowL(*this);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iListBox->SetRect(Rect() );
    iListBox->SetListBoxObserver(this);

    SetListBoxArray();

    iListBox->HandleItemAdditionL();

    iListBox->ActivateL();
    DrawNow();
}

int CQuodDbContainer::GetCurrentItemIdx()
{
    if( iListBox )
    {
        return iListBox->CurrentItemIndex();
    }

    return (-1);
}
// End of File
