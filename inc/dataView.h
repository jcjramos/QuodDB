/*------------------------------------------------------------------------------

            Generic Symbian Data View

           For DbMaster generated files

    JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef ___DATAVIEW_H___
#define ___DATAVIEW_H___

#include <aknview.h>

// NAvigation controls
#include <aknnavi.h>
#include <akntabgrp.h>
#include <aknnavide.h>

#include "listBoxContainer.h"
#include "dbManipulator.h"

#define E_data_view_id (4657)

class data_view_c;



class data_view_c;
// INherit from this guy to know stuff about the ListBox
class data_view_list_box_container_enhanced_observer_c: public list_box_container_enhanced_observer_c
{
    data_view_c *daddy;

public:
    data_view_list_box_container_enhanced_observer_c( data_view_c *daddy );

    virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    virtual void HandleListBoxEventL( CEikListBox* aListBox, MEikListBoxObserver::TListBoxEvent aListBoxEvent );
};


class data_view_handler_c
{
public:
    virtual bool HandleCommandL(data_view_c *dv, TInt aCommand) //Return true if the command was handled ...
    {
        return ( false );
    }

//    virtual int gimme_a_resource_id( data_view_resources_e res ) = 0;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


class data_view_c : public CAknView
{
    friend class data_view_list_box_container_enhanced_observer_c;

public: // constructors and destructor
	static data_view_c* NewL( data_view_handler_c *handler  );
	static data_view_c* NewLC( data_view_handler_c *handler );

    data_view_c( data_view_handler_c *handler );
	~data_view_c();

	TUid Id() const;

    void ConstructL( int res_id = 0 );


    void set_edit_mode( bool on_or_off );
    void set_full_options( bool on_or_off );

    bool get_edit_mode()
    {
        return ( edit_mode );
    }

    void RebuildList( bool redraw = true );
    void RebuildHeader();

    data_view_handler_c *Handler()
    {
        return handler;
    }

    database_manipulator_c &get_database_manipulator()
    {
        return dbmanip;
    }

protected: // from CAknView
	void HandleCommandL(TInt aCommand);
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	virtual void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,	const TDesC8& aCustomMessage);

	void DoDeactivate();

    void ChangeTitleText( int resource_id , TPtrC extra );

    void SaveListPosition();
    void ResumeListPosition();

    list_box_container_c *GetContainer()
    {
        return iContainer;
    }

    database_manipulator_ui_c *GetDbManipulator()
    {
        return &dbmanip;
    }

protected: // data
    list_box_container_c *iContainer; // what this view will display
    data_view_handler_c *handler;

    CAknNavigationControlContainer* iNaviPane;
    CAknTabGroup*                   iTabGroup;
    CAknNavigationDecorator*        iDecoratedTabGroup;


    data_view_list_box_container_enhanced_observer_c observer;

    bool edit_mode;
    bool full_options;

    int list_top_item;
    int list_cur_item;

    database_manipulator_ui_c dbmanip;
};

#endif
