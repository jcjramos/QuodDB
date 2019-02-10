/*------------------------------------------------------------------------------

                        QuodDB Main View


    JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef QUODDBVIEWMAIN_H
#define QUODDBVIEWMAIN_H

#include <aknview.h>

#include "quoddbmain.h"
#include "listBoxContainer.h"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class CQuodDbViewMain;

class CQuodDbViewMainContainer: public CCoeControl
	{
public: // constructors and destructor
	static CQuodDbViewMainContainer* NewL(const TRect& aRect , CQuodDbViewMain * view );
	static CQuodDbViewMainContainer* NewLC(const TRect& aRect , CQuodDbViewMain * view);

    CQuodDbViewMainContainer( CQuodDbViewMain * view );
	~CQuodDbViewMainContainer();

	void ConstructL(const TRect& aRect);

private: // from CoeControl
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void Draw(const TRect& aRect) const;
	void SizeChanged();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

private: // data
    CQuodDbViewMain * view;
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class CQuodDbViewMain : public CAknView
{
public: // constructors and destructor
	static CQuodDbViewMain* NewL( quod_db_main_c *main_app );
	static CQuodDbViewMain* NewLC( quod_db_main_c *main_app );

    CQuodDbViewMain( quod_db_main_c *main_app );
	~CQuodDbViewMain();

	TUid Id() const;

    void ConstructL();


    quod_db_main_c  *MainApp()
    {
        return main_app;
    }

private: // from CAknView
	void HandleCommandL(TInt aCommand);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,	const TDesC8& aCustomMessage);

	void DoDeactivate();

    void ChangeTitleText( int resource_id, TPtrC extra );
    void ChangeTitleText( int resource_id )
    {
        TPtrC nada;
        ChangeTitleText( resource_id, nada );
    }

    void SwitchTable();

private: // data
	TUid				iId;
    quod_db_main_c       *main_app;
    CQuodDbViewMainContainer *iContainer; // what this view will display
};

#endif