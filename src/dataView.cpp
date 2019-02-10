/*------------------------------------------------------------------------------

            Generic Symbian Data View

           For DbMaster generated files

    JCR 3.2004
------------------------------------------------------------------------------*/

#include "dataView.h"
#include "DbMaster.h"

#include <eikmenup.h>						// CEikMenuPane
#include <eikbtgpc.h>

#include <aknviewappui.h>					// CAknViewAppUi


#include <avkon.rsg>
#include <avkon.hrh>
#include <akntitle.h>


#include "dataView.hrh"



#include "genAppRes.h"

#ifndef __GEN_APP_RESOURCE_INCLUDE_FILE__
#error  Please create a GenAppRes.H file including the AppName.RSH & RSH files
#error  Then include dataView.RSS in your application resource file!
#endif


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

data_view_c* data_view_c::NewL( data_view_handler_c *handler )
{
	data_view_c* self = data_view_c::NewLC( handler );
	CleanupStack::Pop(self);
	return self;
}

data_view_c* data_view_c::NewLC( data_view_handler_c *handler )
{
	data_view_c* self = new (ELeave) data_view_c( handler );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

data_view_c::data_view_c( data_view_handler_c * _handler ):observer( this )
{
    handler = _handler;

    iNaviPane = 0;
    iTabGroup = 0;
    iDecoratedTabGroup = 0;

    edit_mode = true;
    full_options = false;

    list_top_item = 0;
    list_cur_item = 0;
}

data_view_c::~data_view_c()
{
}

void data_view_c::ConstructL( int res_id  )
{
	BaseConstructL( res_id ? res_id : R_DATA_VIEW_MASTER_VIEW );

    CEikStatusPane* sp = AppUi()->StatusPane();
    iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

}

TUid data_view_c::Id() const
{
	return TUid::Uid( E_data_view_id );
}


void data_view_c::DoActivateL(const TVwsViewId& , TUid ,	 const TDesC8& )
{
    if( !dbmanip.live( edit_mode, (edit_mode == false || full_options == true) ? true : false ) )
            return;

    iDecoratedTabGroup = iNaviPane->CreateTabGroupL();
    iTabGroup = (CAknTabGroup*) iDecoratedTabGroup->DecoratedControl();

    iTabGroup->SetTabFixedWidthL(KTabWidthWithTwoTabs);

    for( int n = 0; n < dbmanip.n_visible_columns(); n++ )
    {
        dbm_field_c *ff = dbmanip.get_visible_column( n );

        iTabGroup->AddTabL( ff->id(), ff->name() );
    }

    iTabGroup->SetActiveTabByIndex(0);

    iNaviPane->PushL( *iDecoratedTabGroup );


	if (!iContainer)
    {
        iContainer = new (ELeave) list_box_container_c( &observer );
        iContainer->SetMopParent(this); // ATT: Mudar a ordem deste gajo faz desaparecer as setas !!!
        iContainer->ConstructL( ClientRect() );

		AppUi()->AddToStackL(*this, iContainer);
	}

    RebuildList();

    int res_id = edit_mode ? R_DATA_VIEW_STR_DATA_EDITION : R_DATA_VIEW_STR_DATA_REPORT;
    ChangeTitleText( res_id, dbmanip.get_file()->name() );
}


void data_view_c::DoDeactivate()
{
    iNaviPane->Pop( iDecoratedTabGroup );

    delete iDecoratedTabGroup;
    iDecoratedTabGroup = 0;
    iTabGroup = 0;

	if (iContainer)
	{
		AppUi()->RemoveFromStack(iContainer);
		delete iContainer;
		iContainer = NULL;
	}

    dbmanip.die();
}

void data_view_c::RebuildHeader()
{
    iNaviPane->Pop( iDecoratedTabGroup );

    for( int t = 0; t < iTabGroup->TabCount(); )
    {
        int id = iTabGroup->TabIdFromIndex( t );

        iTabGroup->DeleteTabL( id );
    }

    for( int n = 0; n < dbmanip.n_visible_columns(); n++ )
    {
        dbm_field_c *ff = dbmanip.get_visible_column( n );

        iTabGroup->AddTabL( ff->id(), ff->name() );
    }

    iNaviPane->PushL( *iDecoratedTabGroup );

    iTabGroup->SetActiveTabByIndex(0);
}

void data_view_c::RebuildList( bool redraw )
{
    TInt active = iTabGroup ? iTabGroup->ActiveTabIndex() : 0;
    CDesCArrayFlat *tArray = dbmanip.reload_as_array( active );

    if( tArray )
    {
      iContainer->Rebuild( dbmanip.get_use_heading() ? LBCT_HEADER : LBCT_NO_HEADER, tArray, ELbmOwnsItemArray , redraw );
    }
}


void data_view_c::HandleCommandL(TInt aCommand)
{
    bool done = false;

    if( handler )
        done = handler->HandleCommandL( this,aCommand );

    if( done )
        return;

    switch( aCommand )
    {
    	case EDataViewMenuNew:
                {
                    bool first = true;
                    if( iContainer->GetNumListItems() )
                    {
                            SaveListPosition();
                            first = false;
                    }

                    dbmanip.add_line();
                    RebuildList();

                    if( !first )
                    {
                        ResumeListPosition();
                    }
                }
                break;

        case EDataViewMenuEdit:
                {
                    SaveListPosition();

                    if(!edit_mode) // Non editable form doesn't clear the header ...
                            iNaviPane->Pop( iDecoratedTabGroup );

                    dbmanip.edit_line( iContainer ? iContainer->GetCurrentItemIdx() : -1 );

                    if(!edit_mode)
                            iNaviPane->PushL( *iDecoratedTabGroup );

                    RebuildList();

                    ResumeListPosition();
                }
                break;

        case EDataViewMenuDelete:
                {
                    dbmanip.delete_line( iContainer ? iContainer->GetCurrentItemIdx() : -1 );
                    RebuildList();
                }
                break;

        case EDataViewMenuHeader:
                {
                    if( dbmanip.select_header() )
                            RebuildList();
                }
                break;

        case EDataViewMenuSort:
                {
                    if( dbmanip.select_sort() )
                            RebuildList();
                }
                break;

        case EDataViewMenuPages:
                {
                    if( dbmanip.select_pages() )
                    {
                        RebuildHeader();
                        RebuildList();
                    }
                }
                break;

        case EDataViewMenuFiltersAdd:
                {
                    if( dbmanip.select_filters_add() )
                                RebuildList();
                }
                break;

        case EDataViewMenuFiltersRemove:
                {
                    if( dbmanip.select_filters_remove() )
                                RebuildList();
                }
                break;

        case EDataViewMenuExternalImport:
                {
                    dbmanip.get_import_export().do_import_csv();
                    RebuildList();
                }
                break;

        case EDataViewMenuExternalExport:
                {
                    dbmanip.get_import_export().do_export_csv();
                }
                break;

        case EDataViewMenuExternalSeparator:
                {
                    dbmanip.get_import_export().select_separator();
                }
                break;

        case EDataViewMenuExternalUndoImport:
                {
                    dbmanip.get_import_export().undo_import();
                    RebuildList();
                }
                break;

        case EDataViewMenuELookup:
                {
                    int ret = dbmanip.select_lookup();

                    if( ret >= 0 )
                    {
                        list_top_item = ret;
                        list_cur_item = ret;

                        ResumeListPosition();
                        
                        if( iContainer )
                            iContainer->DrawNow();
                    }
                    else
                    {
                        HBufC *strTextError = CEikonEnv::Static()->AllocReadResourceL( R_DATA_VIEW_STR_NOT_FOUND );
                        CEikonEnv::Static()->InfoWinL( *strTextError , _L("") );
                        delete strTextError;
                    }
                }
                break;

        default:
            AppUi()->HandleCommandL(aCommand);
            break;
    }
}


void data_view_c::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{

   if (aResourceId == R_DATA_VIEW_MAIN_MENU_EXTERNAL )
   {
        if( !dbmanip.get_import_export().possible_undo() )
        {
            aMenuPane->SetItemDimmed( EDataViewMenuExternalUndoImport,ETrue);
        }
   }

   if (aResourceId == R_DATA_VIEW_MAIN_MENU_FILTERS )
   {
        if( !dbmanip.n_filters() )
        {
            aMenuPane->SetItemDimmed( EDataViewMenuFiltersRemove,ETrue);
        }
   }

   if (aResourceId == R_DATA_VIEW_MAIN_MENU )
   {
      // No Select !
      if( !dbmanip.get_number_of_keys() )
      {
          aMenuPane->SetItemDimmed( EDataViewMenuSort,ETrue);
      }

      if( !edit_mode )
      {
          aMenuPane->SetItemDimmed( EDataViewMenuDelete,ETrue);
          aMenuPane->SetItemDimmed( EDataViewMenuNew,ETrue);
          aMenuPane->SetItemDimmed( EDataViewMenuExternal,ETrue);
      }
      else
      {
            if( !full_options )
              aMenuPane->SetItemDimmed( EDataViewMenuFilters,ETrue);
      }

      if( iContainer && !iContainer->GetNumListItems() )
      {
          aMenuPane->SetItemDimmed( EDataViewMenuDelete,ETrue);
          aMenuPane->SetItemDimmed( EDataViewMenuEdit,ETrue);

          aMenuPane->SetItemDimmed( EDataViewMenuELookup,ETrue);
      }
   }
}

void data_view_c::ChangeTitleText( int resource_id , TPtrC extra )
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



void data_view_c::set_edit_mode( bool on_or_off )
{
    edit_mode = on_or_off;
}

void data_view_c::set_full_options( bool on_or_off )
{
    full_options = on_or_off;
}

void data_view_c::SaveListPosition()
{
    list_top_item = iContainer->GetTopItemIdx();
    list_cur_item = iContainer->GetCurrentItemIdx();
}

void data_view_c::ResumeListPosition()
{
    if(!iContainer )
            return;

    if( list_top_item >= iContainer->GetNumListItems()
     || list_cur_item >= iContainer->GetNumListItems() ) //HERE
                return;

    iContainer->SetTopItemIdx( list_top_item );
    iContainer->SetCurrentItemIdx( list_cur_item );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

data_view_list_box_container_enhanced_observer_c::data_view_list_box_container_enhanced_observer_c( data_view_c *_daddy )
{
    daddy = _daddy;
}

void data_view_list_box_container_enhanced_observer_c::HandleListBoxEventL( CEikListBox* aListBox, MEikListBoxObserver::TListBoxEvent aListBoxEvent )
{

};

TKeyResponse data_view_list_box_container_enhanced_observer_c::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType == EEventKey)
    {

        if (daddy->iTabGroup == NULL || daddy->iContainer->GetNumListItems() == 0 )
        {
            return EKeyWasNotConsumed;
        }

        TInt active = daddy->iTabGroup->ActiveTabIndex();
        TInt count = daddy->iTabGroup->TabCount();

        switch (aKeyEvent.iCode)
        {
            case EKeyDevice3: // OK
              {
                    daddy->HandleCommandL(EDataViewMenuEdit);
                    return EKeyWasConsumed;
              }


            case EKeyLeftArrow:
            if (active > 0)
            {
                daddy->SaveListPosition();

                active--;
                daddy->iTabGroup->SetActiveTabByIndex(active);
                daddy->RebuildList(false);

                daddy->ResumeListPosition();

                if(daddy->iContainer )
                        daddy->iContainer->Redraw();
                
                return EKeyWasConsumed;
             }
            break;

        case EKeyRightArrow:
            if((active + 1) < count)
            {
                daddy->SaveListPosition();

                active++;
                daddy->iTabGroup->SetActiveTabByIndex(active);
                daddy->RebuildList( false );

                daddy->ResumeListPosition();

                if(daddy->iContainer )
                        daddy->iContainer->Redraw();

                return EKeyWasConsumed;
            }
            break;

        default:
            break;
        }
    }

    return EKeyWasNotConsumed;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// End of File
