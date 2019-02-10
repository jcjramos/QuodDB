/*------------------------------------------------------------------------------

                        QuodDB
                    Field Defenition Form


    JCR 3.2004
------------------------------------------------------------------------------*/
#include "QuodDbFieldFormDef.h"
#include "dbMaster.h"

#include "quoddb.hrh"
#include <quoddb.rsg>
// System includes
#include <AknPopupFieldText.h> // CAknPopupFieldText
#include <aknslider.h> // CAknSlider
#include <avkon.hrh> // Commands
#include <avkon.rsg> // R_AVKON_FORM_MENUPANE
#include <eikedwin.h> // CEikEdwin
#include <eikmenup.h> // CEikMenuPane
#include <eikmfne.h> // Number Editor
#include <aknquerydialog.h>

#include <aknselectionlist.h>
#include <aknlists.h>
// CONSTANTS


// ================= MEMBER FUNCTIONS =======================

/**
* Symbian OS 2 phase constructor.
* Constructs the COpponentFormForm using the NewLC method, popping
* the constructed object from the CleanupStack before returning it.
*
* @return The newly constructed COpponentFormForm
*/
CQuodDbFieldFormDef* CQuodDbFieldFormDef::NewL( dbm_file_c *file, dbm_field_c *field )
{
	CQuodDbFieldFormDef* self = new (ELeave) CQuodDbFieldFormDef( file,field );
	CleanupStack::PushL(self);
	self->ConstructL( R_FIELD_FORM_DEF_MENU_BAR );
	CleanupStack::Pop(self);
	return self;
}

CQuodDbFieldFormDef::CQuodDbFieldFormDef( dbm_file_c *_file, dbm_field_c *_field ):file(_file),field( _field )
{
    tArray = new (ELeave) CDesCArrayFlat(10);

    for( int n = 0; n < field->get_n_extra_descs(); n++ )
    {
        tArray->AppendL( field->get_extra_desc(n) );
    }
    tYesNoArray = 0;
    tEmptyArray = 0;
    iValueTextArray = 0;
    iTextValues = 0;
    in_list_hidden_status = false;
}

CQuodDbFieldFormDef::~CQuodDbFieldFormDef()
{
    delete tYesNoArray;
    delete tEmptyArray;
    delete tArray;
    delete iValueTextArray;
    delete iTextValues;

}

TBool CQuodDbFieldFormDef::SaveFormDataL()
{
    bool new_field = true;
    for( int n = 0; n < file->n_fields(); n++ )
    {
        if( field->id() == file->get_field(n)->id() )
        {
            new_field = false;
            break;
        }
    }

	CEikEdwin* nameEditor = static_cast <CEikEdwin*> (ControlOrNull( EQFieldFormDefName ));
	if (nameEditor)
    {
		HBufC* name = nameEditor->GetTextInHBufL();
		if (name)
        {
             if( new_field  )
             {
                  int len = name->Length();
                  if( len == 0 || !file->valid_field_name( *name ) )
                  {
                      CEikonEnv::Static()->InfoWinL( R_STR_ERROR_INVALID_NAME );
                      return EFalse;
                  }

                  field->set_name( *name );
             }
             else
             {
                 if( *name != field->name() )
                 {
                      CEikonEnv::Static()->InfoWinL( R_DATA_VIEW_STR_ERROR_CANNOT_CHANGE_PROPS );
                 }
             }

			delete name;
		}
        else
        {
            CEikonEnv::Static()->InfoWinL( R_STR_ERROR_INVALID_NAME );
            return EFalse;
        }
	}

    CEikNumberEditor* lenEditor = static_cast <CEikNumberEditor*> (ControlOrNull(EQFieldFormDefLen));
	if (lenEditor)
    {
        field->set_max_len( lenEditor->Number() );
    }


	CAknPopupFieldText* popupFieldType = static_cast <CAknPopupFieldText*> (ControlOrNull(EQFieldFormDefType));
	if( popupFieldType )
    {
        int type = popupFieldType->CurrentValueIndex() + 1;

        if( !new_field && ((dbm_field_type_e)type) != field->type() && file->has_data() )
        {
             CEikonEnv::Static()->InfoWinL( R_DATA_VIEW_STR_ERROR_CANNOT_CHANGE_PROPS );
        }
        else
        {
    		 field->set_type( (dbm_field_type_e)type );
        }
    }

	CAknPopupFieldText* popupFieldKey = static_cast <CAknPopupFieldText*> (ControlOrNull(EQFieldFormDefKey));
	if( popupFieldKey )
    {
        int type = popupFieldKey->CurrentValueIndex();

		field->set_key_type( (dbm_field_key_type_e ) type );
    }



    field->reset_extra_descs();
    if( field->type() == DFT_LIST  )
    {
        if( !tArray->Count() )
        {
            CEikonEnv::Static()->InfoWinL( R_STR_ERROR_NO_OPTIONS_IN_THE_LIST );
            return EFalse;
        }

        for( int d = 0; d < tArray->Count(); d++ )
                field->add_extra_desc( (*tArray) [d] );
    }
    else
    if( field->type() == DFT_YES_NO )
    {
        for( int d = 0; d < tYesNoArray->Count(); d++ )
                field->add_extra_desc( (*tYesNoArray) [d] );
    }

    if( new_field )
    {
       file->insert_field( field ); //New field inserted !
    }

	return ETrue;
}

void CQuodDbFieldFormDef::LoadFormValuesFromDataL()
{
	CEikEdwin* nameEditor = static_cast <CEikEdwin*> (ControlOrNull(EQFieldFormDefName));
	if (nameEditor)
    {
		HBufC* name = field->name().AllocLC();
		nameEditor->SetTextL( name );
        CleanupStack::PopAndDestroy(name);
    }

    CEikNumberEditor* lenEditor = static_cast <CEikNumberEditor*> (ControlOrNull(EQFieldFormDefLen));
    if (lenEditor)
    {
            lenEditor->SetNumber( field->max_len() );
    }


	CAknPopupFieldText* popupFieldType = static_cast <CAknPopupFieldText*> (ControlOrNull(EQFieldFormDefType));
	if( popupFieldType )
    {
		popupFieldType->SetCurrentValueIndex( ((int)field->type()) -1 );
    }
    
	CAknPopupFieldText* popupFieldKey = static_cast <CAknPopupFieldText*> (ControlOrNull(EQFieldFormDefKey));
	if( popupFieldKey )
    {
        popupFieldKey->SetCurrentValueIndex( (int)field->get_key_type() );
    }



	CAknPopupField* popupFieldLo = static_cast <CAknPopupField*> (ControlOrNull(EQFieldFormDefListOptions));
	if( popupFieldLo )
    {
        tEmptyArray = iCoeEnv->ReadDesCArrayResourceL( R_FIELD_FORM_DEF_FIELD_LISTOPTIONS_TEXTARRAY );
        tYesNoArray = iCoeEnv->ReadDesCArrayResourceL( R_FIELD_FORM_ARRAY_YES_NO );

        iValueTextArray = CAknQueryValueTextArray::NewL();


        if( field->type() != DFT_LIST  )
        {
                if( field->type() == DFT_YES_NO )
                {
                    iValueTextArray->SetArray( *tYesNoArray );
                }
                else
                {
                    iValueTextArray->SetArray( *tEmptyArray );
                    in_list_hidden_status = true;
                }

                //SetLineDimmedNow( EQFieldFormDefListOptions, ETrue );
        }
        else
        {
               iValueTextArray->SetArray( *tArray );
        }

        iTextValues = CAknQueryValueText::NewL();
        iTextValues->SetArrayL( iValueTextArray );
        iTextValues->SetCurrentValueIndex( 0 );
        iTextValues->SetQueryCaption( R_QUOD_DB_TEXT );
        // Set values into popup fields.
        popupFieldLo->SetQueryValueL( iTextValues ); // Moved up from below.
    }

}

/**
* Called by QuerySaveChangeL when the user chooses to discard changes made to the form.
* Loads the form values from iOpponent
*/
void CQuodDbFieldFormDef::DoNotSaveFormDataL()
{
	LoadFormValuesFromDataL();
}


/**
* Called by the framework before the form is initialised
* Loads the form values from iOpponent ready for execution of the form
*/
void CQuodDbFieldFormDef::PreLayoutDynInitL()
{

	CAknForm::PreLayoutDynInitL();

	LoadFormValuesFromDataL();

    /*
    SetEditableL(IsEditable()) ;
    SetEditableL(true) ;
    SetChangesPending( false );
    */
}

void CQuodDbFieldFormDef::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	CAknForm::DynInitMenuPaneL(aResourceId,aMenuPane);

	if (aResourceId == R_AVKON_FORM_MENUPANE)
	{
		aMenuPane->SetItemDimmed(EAknFormCmdLabel, ETrue);
		aMenuPane->SetItemDimmed(EAknFormCmdAdd, ETrue);
		aMenuPane->SetItemDimmed(EAknFormCmdDelete, ETrue);

        if( current_type != DFT_LIST  )
        {
            aMenuPane->SetItemDimmed(EMenuNew, ETrue);
            aMenuPane->SetItemDimmed(EMenuDelete, ETrue);
        }

	}
}

void CQuodDbFieldFormDef::ProcessCommandL( TInt aCommandId )
{
    CAknForm::ProcessCommandL( aCommandId );

    if( aCommandId == EMenuNew )
    {
        TBuf<255> textData(NULL);

        HBufC* strText = iEikonEnv->AllocReadResourceL( R_NEW_AVA_OPTION );
        TBuf <1024> text( *strText );

        CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog( textData, CAknQueryDialog::ENoTone );
        if (dlg->ExecuteLD( R_QUODB_DATA_QUERY_DIALOG_DB , text ))
        {
            //CAknPopupField* popupFieldLo = static_cast <CAknPopupField*> (ControlOrNull(EQFieldFormDefListOptions));
            AddItemToList( textData );
        }

        delete strText;
    }

    if( aCommandId == EMenuDelete )
    {
            HBufC* strText = iEikonEnv->AllocReadResourceL( R_DELETE_AVA_OPTION );
            TBuf <1024> text( *strText );

            text.Append( (*tArray)[ iTextValues->CurrentValueIndex() ] );

            CAknQueryDialog *dlg = new CAknQueryDialog();
            if (dlg->ExecuteLD( R_QUODB_CONFIRMATION_YES_NO_DLG, text ) )
            {
                tArray->Delete( iTextValues->CurrentValueIndex() );

                if( !tArray->Count() )
                {
                    iValueTextArray->SetArray( *tEmptyArray );
                }

                ItemListChanged();
            }
    }
}

void CQuodDbFieldFormDef::PrepareForFocusTransitionL()
{
    CAknForm::PrepareForFocusTransitionL();

	CAknPopupFieldText* popupFieldType = static_cast <CAknPopupFieldText*> (ControlOrNull(EQFieldFormDefType));
	if( popupFieldType )
    {
        current_type = popupFieldType->CurrentValueIndex() + 1;

        if( current_type != DFT_LIST  )
        {
            if( !in_list_hidden_status )
            {
                //SetLineDimmedNow( EQFieldFormDefListOptions, ETrue );
                in_list_hidden_status = true;
                //DrawNow();

                //ESTUDAR: virtual void HandleControlStateChangeL(TInt aControlId);
            }
        }
        else
        {
            if( in_list_hidden_status )
            {
                //SetLineDimmedNow( EQFieldFormDefListOptions, EFalse );
                in_list_hidden_status = false;
                //DrawNow();
            }
        }

    }

}

void CQuodDbFieldFormDef::AddItemToList( TPtrC textData )
{
    CAknPopupField* popupFieldLo = static_cast <CAknPopupField*> (ControlOrNull(EQFieldFormDefListOptions));

    if( !popupFieldLo )
            return;

    tArray->AppendL( textData );

    iValueTextArray->SetArray( *tArray );

    ItemListChanged();
}

void CQuodDbFieldFormDef::ItemListChanged()
{
    CAknPopupField* popupFieldLo = static_cast <CAknPopupField*> (ControlOrNull(EQFieldFormDefListOptions));

    if( !popupFieldLo )
            return;

    popupFieldLo->ActivateL();
    popupFieldLo->ActivateSelectionListL();

    DrawNow();
}
