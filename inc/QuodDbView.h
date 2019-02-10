/*------------------------------------------------------------------------------

                Main View For QuodDB application

    JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef QUODDBVIEW_H
#define QUODDBVIEW_H

#include <aknview.h>

#include "quoddbmain.h"
#include "listBoxContainer.h"
#include "dbImportExport.h"

class CQuodDbContainer;
class CQuodDbView;


// INherit from this guy to know stuff about the ListBox
class CQuodDbView_list_box_container_enhanced_observer_c: public list_box_container_enhanced_observer_c
{
    CQuodDbView *daddy;

public:
    CQuodDbView_list_box_container_enhanced_observer_c( CQuodDbView *daddy );

    virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    virtual void HandleListBoxEventL( CEikListBox* aListBox, MEikListBoxObserver::TListBoxEvent aListBoxEvent );
};


class CQuodDbView : public CAknView
{
    friend class CQuodDbView_list_box_container_enhanced_observer_c;
    
public: // constructors and destructor
	static CQuodDbView* NewL( quod_db_main_c *main_app );
	static CQuodDbView* NewLC( quod_db_main_c *main_app );

    CQuodDbView( quod_db_main_c *main_app );
	~CQuodDbView();

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

    // Util
    void ChangeMode( QuodDataEdition_Modes_e new_mode );
    void UpdateContainer();

    void SomethingEditData();
    void SomethingNew();
    void SomethingToDelete();
    void SomethingSelected();

    void ImportCsvStructure();

    void ChangeTitleText( int resource_id, TPtrC extra );
    void ChangeTitleText( int resource_id )
    {
        TPtrC nada;
        ChangeTitleText( resource_id, nada );
    }

private: // data
	TUid				iId;
    list_box_container_c *iContainer; // what this view will display
    quod_db_main_c       *main_app;

    CQuodDbView_list_box_container_enhanced_observer_c observer;

    db_import_export_c import_export;
};

#endif	// __ALARMORGANISER_DEFAULT_VIEW_H__

