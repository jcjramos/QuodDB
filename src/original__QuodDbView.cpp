/**
*
* @brief Definition of CQuodDbContainer
*
* Copyright (c) EMCC Software Ltd 2003
* @version 1.0
*/

// INCLUDE FILES

// Class include
#include "QuodDbView.h"

// System includes
#include <aknselectionlist.h> // CAknSelectionList
#include <akniconarray.h> // CAknIcon
#include <aknnavi.h> 
#include <aknnavide.h> 

#include <QuodDb.mbg> // icons
#include <QuodDb.rsg> // resources
#include <stringloader.h> // StringLoader

#include <EikClb.h>


#include "QuodDbContainer.h"
#include "QuodDbAppUi.h"

// CONSTANTS
const TInt KNumberOfIcons(2);

// ================= MEMBER FUNCTIONS =======================

CQuodDbView::CQuodDbView()
{
    iContainer = 0;
    iNaviPane = 0;
    iDecoratedTabGroup = 0;
    iTabGroup = 0;
}

CQuodDbView::~CQuodDbView()
{
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    if ( iNaviPane )
        {
        if ( iNaviPane->Top() == iDecoratedTabGroup )
            {
            iNaviPane->Pop( iDecoratedTabGroup );
            delete iDecoratedTabGroup;
            }
        }

    delete iContainer; // Deletes container class object.
}
void CQuodDbView::ConstructL(const TRect& aRect)
	{
    BaseConstructL( R_AKNEXLIST_VIEW_COLUMNVIEW );

//	CreateWindowL();
//	SetRect(aRect);

//    ActivateL();
/*
	TInt openedItem(0);

	// Get the name of the file containing the icons
	HBufC* iconFileName;
	iconFileName = StringLoader::LoadLC(R_ICON_FILE_NAME);	// Pushes iconFileName onto the Cleanup Stack.

	// Construct and prepare the dialog
	CAknSelectionListDialog* dialog = CAknSelectionListDialog::NewL(openedItem, NULL, 0);
	dialog->PrepareLC ( R_LISTDLG_DIALOG );

	// Create an array of icons, reading them from the file and set them in the dialog
	CArrayPtr<CGulIcon>* icons = new(ELeave) CAknIconArray(KNumberOfIcons);
	CleanupStack::PushL(icons);
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmQuoddb1player, EMbmQuoddb1player_mask));
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmQuoddb2player, EMbmQuoddb2player_mask));

	dialog->SetIconArrayL(icons); // transferring ownership of icons

	CleanupStack::Pop(icons);


	// Execute the dialog
	if (dialog->RunLD ())
		{
		PlaySelectedGame(openedItem);
		}
	CleanupStack::PopAndDestroy(iconFileName);
*/
	}

/**
* Symbian OS 2 phase constructor.
* Constructs the CQuodDbContainer using the NewLC method, popping
* the constructed object from the CleanupStack before returning it.
*
* @param aRect The rectangle for this window
* @return The newly constructed CQuodDbContainer
*/
CQuodDbView* CQuodDbView::NewL(const TRect& aRect)
	{
	CQuodDbView* self = CQuodDbView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

/**
* Symbian OS 2 phase constructor.
* Constructs the CQuodDbView using the constructor and ConstructL
* method, leaving the constructed object on the CleanupStack before returning it.
*
* @param aRect The rectangle for this window
* @return The newly constructed CQuodDbView
*/
CQuodDbView* CQuodDbView::NewLC(const TRect& aRect)
	{
	CQuodDbView* self = new (ELeave) CQuodDbView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

/**
* Called by the framework to draw this control.  Clears the area in
* aRect.
* @param aRect in which to draw
*/
void CQuodDbView::Draw(const TRect& aRect) const
	{
	}

/**
* Plays the currently select game in the iSavedGamesListBox, if one exists.
* This is an empty implementation of this method
*/
void CQuodDbView::PlaySelectedGame(TInt /*aOpenedItem*/)
	{
	}


void CQuodDbView::DoActivateL(  const TVwsViewId &    aPrevViewId,TUid    aCustomMessageId, const TDesC8 &    aCustomMessage )
{
    // Creates container class object.
    if ( !iContainer )
        {
        iContainer = new( ELeave ) CQuodDbContainer( this ) ;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );

        // Adds container to view control stack.
        AppUi()->AddToStackL( *this, iContainer );

        // Displays outline.
        DisplayInitOutlineL();

        iContainer->MakeVisible( ETrue );
        }

}

void CQuodDbView::DoDeactivate()
{


}

void CQuodDbView::HandleCommandL  (  TInt    aCommand  )
{
    switch ( aCommand ) // Command is...
        {
        case EAknSoftkeyBack:
            AppUi()->ProcessCommandL( EEikCmdExit /*EAknCmdExit*/);
            break;
        //case EEikCmdExit:
        case EAknCmdExit:

            AppUi()->ProcessCommandL( EEikCmdExit /*EAknCmdExit*/ );
            break;
        /*
        case EAknExListSoftkeyNextOutline:
            DisplayNextOutlineL();
            break;
        default:
            DeleteTabGroup(); // Deletes tab group.
            STATIC_CAST( CAknExListAppUi*, AppUi() )->SetOutlineId( aCommand );
            SwitchDisplayOutlineL( aCommand );
            break;
        */            
        }

}

void CQuodDbView::DisplayInitOutlineL()
    {
    TInt outlineId( STATIC_CAST( CQuodDbAppUi*, AppUi() )->OutlineId() );

    if ( !iNaviPane )
        {
        iNaviPane = STATIC_CAST(
            CAknNavigationControlContainer*,
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
        }

    iDecoratedTabGroup = iNaviPane->Top();

    if ( iDecoratedTabGroup )
    {
        iTabGroup = STATIC_CAST(CAknTabGroup*, iDecoratedTabGroup->DecoratedControl() );

        if ( !iTabGroup->ActiveTabIndex() ) // Is active tab first tab?
        {
            SetTitlePaneL( outlineId );
        }

      if( iContainer )
      {
          iContainer->DisplayListBoxL( iTabGroup->TabIdFromIndex( iTabGroup->ActiveTabIndex() ) );
      }
    }
    else
    {
        if ( iTabGroup )
        {
            iTabGroup = NULL;
        }

        SetTitlePaneL( outlineId );

        if( iContainer )
          iContainer->DisplayListBoxL( outlineId );
    }
}

// ------------------------------------------------------------------------------
// CAknExListBaseView::SetNavigationPaneL()
// Sets Navigation pane.
// ------------------------------------------------------------------------------
//
void CQuodDbView::SetNavigationPaneL()
{
/*
    if ( !iNaviPane )
    {
        iNaviPane = STATIC_CAST( CAknNavigationControlContainer*,
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    }

    TInt outlineId( STATIC_CAST( CQuodDbAppUi*, AppUi() )->OutlineId() );

    TInt resourceId(0);

    switch ( outlineId )
        {
        case EAknExListCmdOutline26:
            resourceId = R_AKNEXLIST_SELECTION_TAB;
            break;
        case EAknExListCmdOutline30:
            resourceId = R_AKNEXLIST_MULTISELECTION_TAB;
            break;
        case EAknExListCmdOutline41:
            resourceId = R_AKNEXLIST_STYLE_OF_FINDBOX_TAB;
            break;
        case EAknExListCmdOutline06:
        case EAknExListCmdOutline88:
        case EAknExListCmdOutline90:
        case EAknExListCmdOutline91:
        case EAknExListCmdOutline93:
        case EAknExListCmdOutline95:
        case EAknExListCmdOutline97:
        case EAknExListCmdOutline99:
        case EAknExListCmdOutline102:
            resourceId = R_AKNEXLIST_FLAGTEST_TAB;
            break;
        case EAknExListCmdOutline89:
        case EAknExListCmdOutline92:
        case EAknExListCmdOutline94:
        case EAknExListCmdOutline96:
        case EAknExListCmdOutline98:
            resourceId = R_AKNEXLIST_FLAGTEST_WITHS_TAB;
            break;
        default:
            break;
        }

    if ( resourceId )
        {
        if ( !iDecoratedTabGroup ) // Isn't tab group created?
            {
            SetTitlePaneL( outlineId );

            TResourceReader reader;
            iCoeEnv->CreateResourceReaderLC( reader, resourceId );
            iDecoratedTabGroup = iNaviPane->CreateTabGroupL( reader );
            CleanupStack::PopAndDestroy();  // resource reader

            iTabGroup = STATIC_CAST( CAknTabGroup*,
                                     iDecoratedTabGroup->DecoratedControl() );

            iNaviPane->PushL( *iDecoratedTabGroup );
            }
        }
*/
}
void CQuodDbView::SetTitlePaneL( TInt /*aOutlineId*/ )
{
//  IndicateTitlePaneTextL( KAknExListInitTitleId );
}


// End of File
