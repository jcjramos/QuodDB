/*------------------------------------------------------------------------------

                        Data View Form Dialog

    JCR 3.2004
------------------------------------------------------------------------------*/
#include "dbManipulator.h"
#include "dataViewForm.h"
#include "dbMaster.h"

#include <AknPopupFieldText.h> // CAknPopupFieldText
#include <aknslider.h> // CAknSlider
#include <avkon.hrh> // Commands
#include <avkon.rsg> // R_AVKON_FORM_MENUPANE
#include <eikedwin.h> // CEikEdwin
#include <eikmenup.h> // CEikMenuPane
#include <eikmfne.h> // Number Editor
#include <eikcapc.h> // Number Editor
#include <eikmfne.h>
#include <aknquerydialog.h>
#include <aknselectionlist.h>
#include <aknlists.h>
#include <barsread.h>
#include <etel.h>

#include "genAppRes.h"
#include "dataview.hrh"

#ifndef __GEN_APP_RESOURCE_INCLUDE_FILE__
#error  Please create a GenAppRes.H file including the AppName.RSH & RSH files
#error  Then include dataView.RSS in your application resource file!
#endif

_LIT (KTsyName,"phonetsy.tsy");


class myobserver_c:public MAknPopupFieldObserver
{
    virtual void HandlePopupFieldEventL  (  CAknPopupField *    aPopupField, TAknPopupFieldEvent    aEventType,TInt    aHint  )
    {
    }
};

data_view_form_c* data_view_form_c::NewL( database_manipulator_c *dv,dbm_file_c *field,  data_view_form_type_e type )
{
	data_view_form_c* self = new (ELeave) data_view_form_c( dv, field , type );
	CleanupStack::PushL(self);

    if( self->dialing_mode() )
            self->ConstructL( R_DATA_VIEW_FORM_MENU_BAR );
        else
            self->ConstructL();

	CleanupStack::Pop(self);
	return self;
}

data_view_form_c::data_view_form_c( database_manipulator_c *_daddy, dbm_file_c *_file , data_view_form_type_e _type ):file( _file )
{
    daddy = _daddy;
    type = _type;
    _dialing_field_id = 0;

    if( !daddy->get_is_editable() || type !=  DVFT_NEW_ITEM )
    {
        for( int n = 0; n < file->n_fields(); n++ )
        {
            if( file->get_field(n)->is_hidden() )
                    continue;

            if( file->get_field(n)->type() == DFT_PHONE_NUMBER )
            {
                _dialing_field_id = file->get_field(n)->id();
                break;
            }
        }
    }

}

data_view_form_c::~data_view_form_c()
{
    for( int n = 0; n < list_items.Count(); n++ )
    {
        delete list_items[n].tArray;
        delete list_items[n].iValueTextArray;
        delete list_items[n].iTextValues;
    }

    list_items.Close();

    //daddy->RebuildList();
}

void data_view_form_c::PrepareLC()
{
    int res = 0;

    if( daddy->get_is_editable() )
    {
        res = R_DATA_VIEW_FIELD_EDIT_FORM_DIALOG;
    }
    else
    {
        // HAs Phone numbers ?
        if( dialing_mode() )
                res = R_DATA_VIEW_FIELD_EDIT_FORM_DIALOG_NON_EDITABLE_WOPTIONS;
            else
                res = R_DATA_VIEW_FIELD_EDIT_FORM_DIALOG_NON_EDITABLE;
    }

    CAknForm::PrepareLC( res );

    if(  file  )
    {
        int first_idx = -1;
        for( int n = 0; n < file->n_fields(); n++ )
        {
            if( file->get_field(n)->is_hidden() )
                    continue;

            AddField( file->get_field(n) );

            if( first_idx < 0 )
                    first_idx = n;
        }

        if( file->n_fields() )
        {
            // Torna o primeiro corrente :
            TryChangeFocusToL( file->get_field(first_idx)->id() );
        }
    }

}


TBool data_view_form_c::SaveFormDataL()
{
    if( file )
    {
        int original_type = type;

        if( type == DVFT_NEW_ITEM )
        {
            file->get_table().Reset();

            file->get_table().InsertL();
            type = DVFT_EDIT_ITEM;
        }
        else
        {
            file->get_table().UpdateL();
        }

        for( int n = 0; n < file->n_fields(); n++ )
        {
            if( file->get_field(n)->is_hidden() )
                    continue;
        
            SaveField( file->get_field(n) );
        }

        int e;
        TRAP( e, file->get_table().PutL() );

        if( e )
        {
            CEikonEnv::Static()->InfoMsg( R_DATA_VIEW_STR_ERROR_UPDATING_TABLE );

            file->get_table().Cancel();

            type = ( data_view_form_type_e) original_type;
        }
    }


	return ETrue;
}

void data_view_form_c::SetFieldData( dbm_field_c *field )
{
    if( field->get_col_idx_in_table() == KDbNullColNo || file->get_table().IsColNull(field->get_col_idx_in_table()) )
    {
        return;
    }

    stunt_item.build_from_db( field , file );
    stunt_item.update_str( field );

    switch( field->type() )
    {
        case DFT_VOID:
        case DFT_INT:
            {
                CAknIntegerEdwin* intEditor = static_cast <CAknIntegerEdwin*> (ControlOrNull( field->id() ));
                if (intEditor)
                {
                      intEditor->SetValueL( stunt_item.get_as_num() );
                }

            }
            break;

        case DFT_TEXT:
        case DFT_BIGNUMBER:
        case DFT_PHONE_NUMBER:
        case DFT_FLOAT:
        case DFT_TEXT_NUMBER:
            {
                  CEikEdwin* nameEditor = static_cast <CEikEdwin*> (ControlOrNull( field->id() ));
                  if (nameEditor)
                  {
                        TBuf <256> bf = stunt_item.get_as_buff();
                		nameEditor->SetTextL( &bf );
                  }
            }
            break;

        case DFT_DATE:
            {
                CEikDateEditor* dateEditor = static_cast <CEikDateEditor*> (ControlOrNull( field->id() ));
                if( dateEditor )
                {
                    dateEditor->SetDate( stunt_item.get_as_ttime() );
                }

            }
            break;

        case DFT_TIME:
            {
                CEikTimeEditor* timeEditor = static_cast <CEikTimeEditor*> (ControlOrNull( field->id() ));
                if( timeEditor )
                {
                    timeEditor->SetTime( stunt_item.get_as_ttime() );
                }

            }
            break;

        case DFT_DATE_TIME:
            {
                CEikTimeAndDateEditor *dtimeEditor = static_cast <CEikTimeAndDateEditor*> (ControlOrNull( field->id() ));
                if( dtimeEditor )
                {
                    dtimeEditor->SetTimeAndDate( stunt_item.get_as_ttime() );
                }

            }
            break;
/*
        case DFT_FLOAT:
            {
                CEikFloatingPointEditor* fpEdwin = static_cast <CEikFloatingPointEditor*> (ControlOrNull( field->id() ));
                if( fpEdwin )
                {
                        TReal xrnum = stunt_item.get_as_rnumber();
                        fpEdwin->SetValueL( &xrnum );
                }
            }
            break;
*/
        case DFT_LIST:
        case DFT_YES_NO:
            {
                for( int n = 0; n < list_items.Count(); n++ )
                {
                    if( list_items[n].id == field->id() )
                    {
                        if( stunt_item.get_as_num() < list_items[n].tArray->Count() )
                            list_items[n].iTextValues->SetCurrentValueIndex( stunt_item.get_as_num() );
                    }
                }
                /*
                CAknPopupField* popup = static_cast <CAknPopupField*> (ControlOrNull( field->id() ));
                if( popup )
                {
                    popup->
                }
                */
            }
            break;

    }

}

void data_view_form_c::SaveField( dbm_field_c *field )
{
    stunt_item.set_as_str(_L(" ")); // To avoid null fields to be filled with last valid information !

    switch( field->type() )
    {
        case DFT_VOID:
        case DFT_INT:
            {
                CAknIntegerEdwin* intEditor = static_cast <CAknIntegerEdwin*> (ControlOrNull( field->id() ));
                if (intEditor)
                {
                     int num = 0;
                     if( intEditor->GetTextAsInteger( num ) == KErrNone )
                              stunt_item.set_as_num( num );
                }
             }
            break;

        case DFT_BIGNUMBER:
        case DFT_PHONE_NUMBER:
        case DFT_TEXT:
        case DFT_FLOAT:
        case DFT_TEXT_NUMBER:
            {
                  CEikEdwin* nameEditor = static_cast <CEikEdwin*> (ControlOrNull( field->id() ));
                  if (nameEditor)
                  {
                      HBufC* name = nameEditor->GetTextInHBufL();
                      if (name && name->Length() )
                      {
                          stunt_item.set_as_str( *name );
                          stunt_item.workout_from_str( field, stunt_item.get_as_str() );

                          delete name;
                      }
                  }
            }
            break;

        case DFT_DATE:
            {
                CEikDateEditor* dateEditor = static_cast <CEikDateEditor*> (ControlOrNull( field->id() ));
                if( dateEditor )
                {
                    TTime tt = dateEditor->Date();
                    stunt_item.set_as_ttime( tt );
                }
            }
            break;

        case DFT_TIME:
            {
                CEikTimeEditor* timeEditor = static_cast <CEikTimeEditor*> (ControlOrNull( field->id() ));
                if( timeEditor )
                {
                    TTime tt = timeEditor->Time();
                    stunt_item.set_as_ttime( tt );
                }
            }
            break;

        case DFT_DATE_TIME:
            {
                CEikTimeAndDateEditor* dtimeEditor = static_cast <CEikTimeAndDateEditor*> (ControlOrNull( field->id() ));
                if( dtimeEditor )
                {
                    TTime tt = dtimeEditor->TimeAndDate();
                    stunt_item.set_as_ttime( tt );
                }
            }
            break;
/*
        case DFT_FLOAT:
            {
                CEikFloatingPointEditor* fpEdwin = static_cast <CEikFloatingPointEditor*> (ControlOrNull( field->id() ));
                if( fpEdwin )
                {
                        TReal rnum = fpEdwin->Value();
                        stunt_item.set_as_rnumber( rnum );
                }

            }
            break;
*/
        case DFT_LIST:
        case DFT_YES_NO:
            {
                stunt_item.set_as_num( 0 );

                for( int n = 0; n < list_items.Count(); n++ )
                {
                    if( list_items[n].id == field->id() )
                    {
                        int idx = list_items[n].iTextValues->CurrentValueIndex();
                        stunt_item.set_as_num( idx );

                        break;
                    }
                }

            }
            break;

    }

    stunt_item.to_db( field , file );
}

void data_view_form_c::LoadFormValuesFromDataL()
{
    if( file && type == DVFT_EDIT_ITEM )
    {
        file->get_table().GetL();

        for( int n = 0; n < file->n_fields(); n++ )
        {
                if( file->get_field(n)->is_hidden() )
                    continue;

                SetFieldData( file->get_field(n) );
        }
    }
}

void data_view_form_c::DoNotSaveFormDataL()
{
	LoadFormValuesFromDataL();
}

void data_view_form_c::PreLayoutDynInitL()
{
	CAknForm::PreLayoutDynInitL();

    LoadFormValuesFromDataL();
}

void data_view_form_c::PostLayoutDynInitL()
{
    CAknForm::PostLayoutDynInitL();
}

void data_view_form_c::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	CAknForm::DynInitMenuPaneL(aResourceId,aMenuPane);

	if (aResourceId == R_AVKON_FORM_MENUPANE)
	{
        if( !daddy->get_is_editable() )
                aMenuPane->SetItemDimmed(EAknFormCmdEdit, ETrue);

		aMenuPane->SetItemDimmed(EAknFormCmdLabel, ETrue);
        aMenuPane->SetItemDimmed(EAknFormCmdAdd, ETrue);
        aMenuPane->SetItemDimmed(EAknFormCmdDelete, ETrue);
    }
}

void data_view_form_c::ProcessCommandL( TInt aCommandId )
{
    CAknForm::ProcessCommandL( aCommandId );

    if( aCommandId == EDataViewMenuFormMenuDial )
    {
        int fid = IdOfFocusControl();
        dbm_field_c *ff = file->find_field( fid );

        if( !ff || ff->type() != DFT_PHONE_NUMBER )
        {
            ff = file->find_field( _dialing_field_id );
            fid = _dialing_field_id;
        }

        if( !ff )
                return;

        CEikEdwin* nameEditor = static_cast <CEikEdwin*> (ControlOrNull( fid ));
        if (nameEditor)
        {
            HBufC* name = nameEditor->GetTextInHBufL();
            DialNumberL( *name );
            delete name;
        }

    }
}

void data_view_form_c::PrepareForFocusTransitionL()
{
    CAknForm::PrepareForFocusTransitionL();
}

void data_view_form_c::AddField( dbm_field_c *field )
{
    int iNewItemId = field->id();

    switch( field->type() )
    {
        case DFT_INT:
            {
                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EAknCtIntegerEdwin, NULL) ;

                CAknIntegerEdwin* myEdwin = STATIC_CAST(CAknIntegerEdwin*, myEdControl) ;

                myEdwin->ConstructL( -10000, 10000, 4 );
            }
            break;

        case DFT_TEXT:
        case DFT_BIGNUMBER:
        case DFT_PHONE_NUMBER:
//        case DFT_FLOAT: is a special !
            {
                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EEikCtEdwin, NULL) ;

                CEikEdwin* myEdwin = STATIC_CAST(CEikEdwin*, myEdControl) ;
                myEdwin->ConstructL( EEikEdwinNoHorizScrolling | EEikEdwinResizable, 10, field->max_len() , 10);

                if( field->type() == DFT_BIGNUMBER || field->type() == DFT_PHONE_NUMBER || field->type() == DFT_FLOAT )
                {
                    TCoeInputCapabilities cap( TCoeInputCapabilities::EWesternNumericReal );

                    myEdwin->SetInputCapabilitiesL( cap );
                }

                myEdwin->CreateTextViewL();
            }
            break;


        case DFT_FLOAT:
            {
                TResourceReader reader;
                //iCoeEnv->CreateResourceReaderLC(reader, R_DATA_VIEW_FIELD_EDIT_FORM_FLOAT_TEXT_NUMBER );
                iCoeEnv->CreateResourceReaderLC(reader, R_DATA_VIEW_FIELD_EDIT_FORM_TEXT_NUMBER );

                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EEikCtEdwin, NULL) ;

                CEikEdwin* myEdwin = STATIC_CAST(CEikEdwin*, myEdControl) ;

                myEdwin->ConstructFromResourceL(reader);

                CleanupStack::PopAndDestroy();  // Resource reader
            }
            break;

        case DFT_TEXT_NUMBER:
            {
                TResourceReader reader;
                iCoeEnv->CreateResourceReaderLC(reader, R_DATA_VIEW_FIELD_EDIT_FORM_TEXT_NUMBER );

                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EEikCtEdwin, NULL) ;

                CEikEdwin* myEdwin = STATIC_CAST(CEikEdwin*, myEdControl) ;

                myEdwin->ConstructFromResourceL(reader);

                CleanupStack::PopAndDestroy();  // Resource reader
            }
            break;

        case DFT_DATE:
            {
                TResourceReader reader;
                iCoeEnv->CreateResourceReaderLC(reader, R_DATA_VIEW_FIELD_EDIT_FORM_DATE_EDITOR );

                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EEikCtDateEditor, NULL) ;

                CEikDateEditor* myEdwin = STATIC_CAST(CEikDateEditor*, myEdControl) ;

                myEdwin->ConstructFromResourceL(reader);

                TTime today;
                today.HomeTime();
                myEdwin->SetDate( today );

                CleanupStack::PopAndDestroy();  // Resource reader
            }
            break;

        case DFT_TIME:
            {
                TResourceReader reader;
                iCoeEnv->CreateResourceReaderLC(reader, R_DATA_VIEW_FIELD_EDIT_FORM_TIME_EDITOR );

                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EEikCtTimeEditor, NULL) ;

                CEikTimeEditor* myEdwin = STATIC_CAST(CEikTimeEditor*, myEdControl) ;

                myEdwin->ConstructFromResourceL(reader);

                TTime today;
                today.HomeTime();
                myEdwin->SetTime( today );

                CleanupStack::PopAndDestroy();  // Resource reader
            }
            break;

        case DFT_DATE_TIME:
            {
                TResourceReader reader;
                iCoeEnv->CreateResourceReaderLC(reader, R_DATA_VIEW_FIELD_EDIT_FORM_DATE_TIME_EDITOR );

                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EEikCtTimeAndDateEditor, NULL) ;

                CEikTimeAndDateEditor* myEdwin = STATIC_CAST(CEikTimeAndDateEditor*, myEdControl) ;

                myEdwin->ConstructFromResourceL(reader);

                TTime today;
                today.HomeTime();
                myEdwin->SetTimeAndDate( today );

                CleanupStack::PopAndDestroy();  // Resource reader
            }
            break;


/*
        case DFT_FLOAT:
            {
                TReal aMin( 0 );
                TReal aMax( 0 );

                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  , EAknCtFloatingPointEdwin, NULL) ;

                CEikFloatingPointEditor* myEdwin = STATIC_CAST(CEikFloatingPointEditor*, myEdControl) ;

                myEdwin->ConstructL( aMin, aMax, 10 );
            }
            break;
*/
        case DFT_LIST:
        case DFT_YES_NO:
            {
                data_view_form_list_item_s dview;
                dview.id = field->id();
                dview.tArray = new CDesCArrayFlat( 10 );
                dview.iValueTextArray = CAknQueryValueTextArray::NewL();
                dview.iTextValues = CAknQueryValueText::NewL();

                // Prepare the array;
                for( int n = 0; n < field->get_n_extra_descs(); n++ )
                {
                    dview.tArray->AppendL( field->get_extra_desc( n ) );
                }

                dview.iValueTextArray->SetArray( *dview.tArray );
                dview.iTextValues->SetArrayL( dview.iValueTextArray );
                dview.iTextValues->SetCurrentValueIndex( 0 );
                dview.iTextValues->SetQueryCaption( R_DATA_VIEW_FIELD_EDIT_FORM_TEXT );


                TResourceReader reader;
                iCoeEnv->CreateResourceReaderLC(reader, R_DATA_VIEW_FIELD_EDIT_FORM_POPUP  );

                CCoeControl* myEdControl = CreateLineByTypeL(field->name(), ActivePageId(),  iNewItemId  ,EAknCtPopupField, NULL) ;

                CAknPopupField* myEdwin = STATIC_CAST(CAknPopupField*, myEdControl) ;

                myEdwin->SetQueryValueL( dview.iTextValues );
                myEdwin->ConstructFromResourceL(reader);

                CleanupStack::PopAndDestroy();  // Resource reader

                list_items.Append( dview );
                UpdatePageL( true );
            }
            break;

        default:CEikonEnv::Static()->InfoMsg( _L("data_view_form_c::AddField() Invalid type x") );

    }
}

TKeyResponse data_view_form_c::OfferKeyEventL(  const TKeyEvent &    aKeyEvent, TEventCode    aType )
{
    TKeyResponse ret = CAknForm::OfferKeyEventL(  aKeyEvent,  aType );

    if (daddy->get_is_editable() && aType == EEventKey)
    {
        if(aKeyEvent.iCode == EKeyDevice3 ) // OK
        {
             int id = IdOfFocusControl();         // Ultra stupid bug fix !
             dbm_field_c *ff = file->find_field( id );

             if( DFT_LIST == ff->type() || DFT_YES_NO ==  ff->type() )
             {
                   CAknPopupField* pp = static_cast <CAknPopupField*> (ControlOrNull( ff->id() ));
                   pp->OfferKeyEventL(  aKeyEvent, aType );
             }
        }
    }

    return ret;
}

void data_view_form_c::DialNumberL(const TDesC& _aPhoneNumber)
    {
    // emulator does not support dialing
    // Limpa caracteres esquisitos :
    TBuf <256> aPhoneNumber;

    for( int n = 0; n < _aPhoneNumber.Length(); n++ )
    {
        TChar ch = _aPhoneNumber[n];

        if( ch.IsDigit() )
        {
            aPhoneNumber.Append( ch );
        }
    }
#ifdef __WINS__
    CEikonEnv::Static()->InfoWinL( _L("CANNOT DIAL ON THE EMMULATOR"), aPhoneNumber );
#else
#ifndef __OS91__
    //Create a connection to the tel server
    RTelServer server;
    CleanupClosePushL(server);
    User::LeaveIfError(server.Connect());

    //Load in the phone device driver
    User::LeaveIfError(server.LoadPhoneModule(KTsyName));
    
    //Find the number of phones available from the tel server
    TInt numberPhones;
    User::LeaveIfError(server.EnumeratePhones(numberPhones));

    //Check there are available phones
    if (numberPhones < 1)
        {
        User::Leave(KErrNotFound);
        }

    //Get info about the first available phone
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(server.GetPhoneInfo(0, info));

    //Use this info to open a connection to the phone, the phone is identified by its name
    RPhone phone;
    CleanupClosePushL(phone);
    User::LeaveIfError(phone.Open(server, info.iName));

    //Get info about the first line from the phone
    RPhone::TLineInfo lineInfo;
    User::LeaveIfError(phone.GetLineInfo(0, lineInfo));

    //Use this to open a line
    RLine line;
    CleanupClosePushL(line);
    User::LeaveIfError(line.Open(phone, lineInfo.iName));

    //Open a new call on this line
    TBuf <100> newCallName;
    RCall call;
    CleanupClosePushL(call);
    User::LeaveIfError(call.OpenNewCall(line, newCallName));

    //newCallName will now contain the name of the call
    User::LeaveIfError(call.Dial(aPhoneNumber));

    //Close the phone, line and call connections and remove them from the cleanup stack
    //NOTE: This does not hang up the call
    CleanupStack::PopAndDestroy(3);//phone, line, call

    //Unload the phone device driver
    User::LeaveIfError(server.UnloadPhoneModule(KTsyName));

    //Close the connection to the tel server and remove it from the cleanup stack
    CleanupStack::PopAndDestroy(&server);
#endif
#endif
    }

