/*------------------------------------------------------------------------------

                        QuodDB Main View


    JCR 3.2004
------------------------------------------------------------------------------*/
#define SW_VERSION _L("QuodDB 1.09j")

#include <quoddb.rsg>

#include "QuodDbViewMain.h"
#include "dbMaster.h"
#include "QuodDbMaster.h"
#include "QuodDb.hrh"
#include "instalationDateChecker.h"

#include <eikmenup.h>						// CEikMenuPane
#include <eikbtgpc.h>

#include <aknviewappui.h>					// CAknViewAppUi
#include <aknselectionlist.h>
#include <aknquerydialog.h>
#include <aknmessagequerydialog.h>

#include <avkon.rsg>
#include <avkon.hrh>
#include <akntitle.h>
#include <sysutil.h>

#define KAknExListGray TRgb(0xaaaaaa)
#define KAknExListWhite TRgb(0xFFFFFF)
#define KAknExListBlack TRgb(0x000000)
#define KAknExListTab '\t'

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CQuodDbViewMain* CQuodDbViewMain::NewL( quod_db_main_c *main_app )
{
	CQuodDbViewMain* self = CQuodDbViewMain::NewLC( main_app );
	CleanupStack::Pop(self);
	return self;
}

CQuodDbViewMain* CQuodDbViewMain::NewLC( quod_db_main_c *main_app)
{
	CQuodDbViewMain* self = new (ELeave) CQuodDbViewMain( main_app );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CQuodDbViewMain::CQuodDbViewMain( quod_db_main_c *_main_app )
{
    main_app = _main_app;
    iContainer = 0;


}

CQuodDbViewMain::~CQuodDbViewMain()
{

}

void CQuodDbViewMain::ConstructL()
{
	BaseConstructL( R_MAIN_VIEW );
}

TUid CQuodDbViewMain::Id() const
{
	return TUid::Uid( EMainViewId );
}

/**
* Called by the framework when the view is activated.  Constructs the container if necessary, setting this view as its MOP parent, and
 adding it to the control stack.
*/
void CQuodDbViewMain::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,	 const TDesC8& /*aCustomMessage*/)
{
#ifdef __DEMONSTRATION_VERSION__
    _LIT( dumb_file_name, "ROMYS09.SYS" ); // Dumb enought !!!

    TBufC <256> dumb( dumb_file_name );

    TTime current_date;
    TTime inst_date;
    current_date.HomeTime();

    instalation_date_checker_c ck;
    inst_date = ck.check_instalation_date( dumb );

    TTimeIntervalDays end( 30 );

    TTime finst( inst_date );
    finst += end;

    if( (current_date > finst) || ( current_date.DateTime().Year() > 2026 && current_date.DateTime().Month() > EAugust ) )
    {
        CEikonEnv::Static()->InfoWinL( _L("Expired Demonstration Version"),_L("please contact www.QuodSoftware.com") );
        AppUi()->HandleCommandL( EEikCmdExit );
    }
    else
    {
        TTimeIntervalDays end( 22 );

        TTime xinst( inst_date );
        xinst += end;

        if( current_date > xinst || ( current_date.DateTime().Year() > 2006 && current_date.DateTime().Month() > EMarch && current_date.DateTime().Day() > 22 ) )
        {
            CEikonEnv::Static()->InfoMsg( _L("DEMO EXPIRES NEXT WEEK !") );
        }
    }
#endif

	if (!iContainer)
    {

        iContainer = new (ELeave) CQuodDbViewMainContainer( this );
        iContainer->SetMopParent(this); // ATT: Mudar a ordem deste gajo faz desaparecer as setas !!!
        iContainer->ConstructL( ClientRect() );

		AppUi()->AddToStackL(*this, iContainer);
	}

    ChangeTitleText( R_TITLE_MOBILE_DATABASE );

    User::LeaveIfError( main_app->load_cfgs() );
}

/**
* Called by the framework when the view is deactivated.Removes the container from the control stack and deletes it.
*/
void CQuodDbViewMain::DoDeactivate()
{
	if (iContainer)
	{
		AppUi()->RemoveFromStack(iContainer);
		delete iContainer;
		iContainer = NULL;
	}
}

void CQuodDbViewMain::HandleCommandL(TInt aCommand)
{

	switch (aCommand)
    {
            case EMenuReportMode: //ANTI HACKER SUPPORT
                    {
#ifdef __DEMONSTRATION_VERSION__
                TTime tm;
                tm.HomeTime();

                if( tm.DayNoInWeek() == EMonday )
                {
                    _LIT( dumb_file_name, "ROMYS02.SYS" ); // Dumb enought !!!

                    TBufC <256> dumb( dumb_file_name );

                    TTime current_date;
                    TTime inst_date;
                    current_date.HomeTime();

                    instalation_date_checker_c ck;
                    inst_date = ck.check_instalation_date( dumb );

                    TTimeIntervalDays end( 30 );

                    TTime finst( inst_date );
                    finst += end;

                    if( (current_date > finst) || ( current_date.DateTime().Year() > 2006 && current_date.DateTime().Month() > EApril ) )
                    {
                          CEikonEnv::Static()->InfoWinL( _L("Expired Demonstration Version"),_L("please contact www.QuodSoftware.com") );
                          AppUi()->HandleCommandL( EEikCmdExit );
                    }
                }
#endif

                    }
                    main_app->switch_mode( QMV_MODE_DATA_REPORT );
                    break;

            case EMenuDataMode:
                    main_app->switch_mode( QMV_MODE_DATA_EDIT );
                    break;

            case EMenuDataDefinitionMode:
                    main_app->switch_mode( QMV_MODE_DATA_DEF );
                    break;

            case EMenuChangeTable:
                    SwitchTable();
                    break;

            case EMenuAbout:
                    {
                        //HBufC *titleText = iEikonEnv->AllocReadResourceL( R_STR_ABOUT_TITLE );  Language dependent -> doesnt make sense for APP NAME
                        HBufC *aboutText = iEikonEnv->AllocReadResourceL( R_STR_ABOUT_TEXT );

                        TBuf <1024> *ttext = new TBuf <1024>;
                        ttext->Append( SW_VERSION );

                        CAknMessageQueryDialog* dlg = new CAknMessageQueryDialog( aboutText, ttext );

                        dlg->PrepareLC(R_AVKON_MESSAGE_QUERY_DIALOG);
                        dlg->RunLD();
                    }
                    break;

            default:
        			AppUi()->HandleCommandL(aCommand);
    }
}

void CQuodDbViewMain::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
    dbm_master_c *current_db = MainApp()->get_db_master()->get_current_db();
    dbm_file_c *current_file = MainApp()->get_db_master()->get_current_file();

    if (aResourceId != R_MAIN_VIEW_MENU )
            return;

    // No Select !
    if( !current_db || !current_file )
    {
        aMenuPane->SetItemDimmed( EMenuReportMode ,ETrue);
        aMenuPane->SetItemDimmed( EMenuDataMode ,ETrue);
        aMenuPane->SetItemDimmed( EMenuChangeTable ,ETrue);
    }

    if( current_file && !current_file->n_fields() )
    {
        aMenuPane->SetItemDimmed( EMenuReportMode ,ETrue);
        aMenuPane->SetItemDimmed( EMenuDataMode ,ETrue);
    }

}



void CQuodDbViewMain::ChangeTitleText( int resource_id , TPtrC extra )
{
    CAknTitlePane* titlePane = STATIC_CAST(
        CAknTitlePane*,
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    if ( !resource_id )
    {
        titlePane->SetTextToDefaultL();
    }
    else
    {
        TBuf<1024> titleText( NULL );
        iEikonEnv->ReadResource( titleText, resource_id );

        if( extra.Length() )
        {
            titleText.Append( extra );
        }

        titlePane->SetTextL( titleText );
    }
}

void CQuodDbViewMain::SwitchTable()
{
    dbm_master_c *current_db = MainApp()->get_db_master()->get_current_db();
    dbm_file_c *current_file = MainApp()->get_db_master()->get_current_file();

    if( !current_db )
                return;

    CDesCArrayFlat *tArray = new (ELeave) CDesCArrayFlat(10);

    for( int n = 0; n < current_db->n_files(); n++ )
    {
        tArray->AppendL( current_db->get_file( n )->name() );
    }

    HBufC * propmtText = iEikonEnv->AllocReadResourceL( R_STR_TABLES_LIST );

    CAknListQueryDialog* dlg;
    TInt index( 0 );
    dlg = new( ELeave ) CAknListQueryDialog( &index );

    dlg->PrepareLC( R_QUODB_VIEW_LIST_QUERY );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    dlg->SetItemTextArray( tArray );
    dlg->SetHeaderTextL( *propmtText );
    TInt answer = dlg->RunLD();

    delete propmtText;

    if( answer )
    {
        current_file = current_db->get_file( index );
        MainApp()->get_db_master()->set_current_file( current_file );

        if( iContainer )
            iContainer->DrawNow();
    }

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define KAknExListGray TRgb(0xaaaaaa)

CQuodDbViewMainContainer* CQuodDbViewMainContainer::NewL(const TRect& aRect , CQuodDbViewMain *view )
{
	CQuodDbViewMainContainer* self = CQuodDbViewMainContainer::NewLC(aRect , view );
	CleanupStack::Pop(self);
	return self;
}

CQuodDbViewMainContainer* CQuodDbViewMainContainer::NewLC(const TRect& aRect, CQuodDbViewMain *view )
{
	CQuodDbViewMainContainer* self = new (ELeave) CQuodDbViewMainContainer( view );
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
}


CQuodDbViewMainContainer::CQuodDbViewMainContainer( CQuodDbViewMain *_view )
{
    view = _view;
}

CQuodDbViewMainContainer::~CQuodDbViewMainContainer()
{

}

void CQuodDbViewMainContainer::ConstructL(const TRect& aRect)
{
	CreateWindowL();

	SetRect(aRect);
	ActivateL();
}

void CQuodDbViewMainContainer::SizeChanged()
{
}

TInt CQuodDbViewMainContainer::CountComponentControls() const
{
	return 0;
}

CCoeControl* CQuodDbViewMainContainer::ComponentControl(TInt aIndex) const
{
	switch (aIndex)
	{
		default:
			return NULL;
	}
}


/**
* Called by the framework to draw this control.  Clears the area in
* aRect.
* @param aRect in which to draw
*/
void CQuodDbViewMainContainer::Draw(const TRect& aRect) const
{
    const int DBORDER = 5;

    TRect dRect = aRect;

    // (Re)draw the rectangle of frame.
    CWindowGc& gc = SystemGc();

    int line_h = CEikonEnv::Static()->LegendFont()->HeightInPixels() + DBORDER;
    int line_hh = CEikonEnv::Static()->TitleFont()->HeightInPixels() + DBORDER;

    gc.UseFont( CEikonEnv::Static()->TitleFont() );


    dRect.iBr.iY -= (line_h * 3) + DBORDER*3;

    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KAknExListGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( dRect );

    dbm_master_c *current_db = view->MainApp()->get_db_master()->get_current_db();
    dbm_file_c *current_file = view->MainApp()->get_db_master()->get_current_file();

    HBufC* strTextDb = iEikonEnv->AllocReadResourceL( R_CURRENT_DATABASE );
    HBufC* strTextTb = iEikonEnv->AllocReadResourceL( R_CURRENT_TABLE );
    HBufC* strTextCurSel = iEikonEnv->AllocReadResourceL( R_CURRENT_SELECTION );
    HBufC* strTextInfo1 = iEikonEnv->AllocReadResourceL( R_MAIN_SCREEN_INFO1 );
    HBufC* strTextInfo2 = iEikonEnv->AllocReadResourceL( R_MAIN_SCREEN_INFO2 );

    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
#ifdef __DEMONSTRATION_VERSION__
    _LIT( demoVer, "Demonstration Version"  );
    gc.DrawText( demoVer, dRect, line_h*2 , CGraphicsContext::ECenter );
#else
    gc.DrawText( *strTextInfo1, dRect, line_h*2 , CGraphicsContext::ECenter );
#endif

    gc.DiscardFont();
    gc.UseFont( CEikonEnv::Static()->LegendFont() );

    gc.DrawText( *strTextInfo2, dRect, line_hh*3 , CGraphicsContext::ECenter );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    TBuf <1024> textDb( *strTextDb );
    TBuf <1024> textTb( *strTextTb );

    delete strTextDb;
    delete strTextTb;
    delete strTextInfo1;
    delete strTextInfo2;

    if( current_db )
        textDb.Append( current_db->name() );

    if( current_file )
        textTb.Append( current_file->name() );

    gc.SetPenSize( TSize(3,3) );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetPenColor( KAknExListBlack );
    gc.SetBrushColor( KAknExListWhite );


    TRect r( dRect.iTl.iX , dRect.iBr.iY ,aRect.iBr.iX, aRect.iBr.iY );
    gc.DrawRect( r );
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.DrawRect( r );

    gc.SetPenSize( TSize(1,1) );

    TPoint lnS( r.iTl.iX, r.iTl.iY + line_h + DBORDER  );
    TPoint lnE( r.iBr.iX, r.iTl.iY + line_h + DBORDER );
    gc.DrawLine( lnS, lnE );

    gc.DrawText( *strTextCurSel, r, line_h , CGraphicsContext::ECenter );
//    TPoint ptC(dRect.iTl.iX + DBORDER, dRect.iBr.iY + line_h );
//    gc.DrawText( *strTextCurSel, ptC );

    TPoint pt1(dRect.iTl.iX + DBORDER , dRect.iBr.iY + DBORDER + line_h * 2 );
    gc.DrawText( textDb, pt1 );
    TPoint pt2(dRect.iTl.iX + DBORDER, dRect.iBr.iY + DBORDER + line_h * 3 );
    gc.DrawText( textTb, pt2 );

    gc.DiscardFont();

    delete strTextCurSel;
}


TKeyResponse CQuodDbViewMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    /*
    if (iListBox)
    {
		return iListBox->OfferKeyEventL(aKeyEvent, aType);
    }
    */

	return EKeyWasNotConsumed;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// End of File
