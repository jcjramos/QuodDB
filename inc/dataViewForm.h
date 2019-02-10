/*------------------------------------------------------------------------------

                    Data View Form

    JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef DATAVIEWFORM_H
#define DATAVIEWFORM_H

// System includes
#include <aknform.h> // CAknForm
#include "dbDataItem.h"

class dbm_file_c;
class dbm_field_c;

class CAknQueryValueTextArray;
class CAknQueryValueText;
class data_view_handler_c;
class database_manipulator_c;

struct data_view_form_list_item_s
{
    int id;
    CDesCArrayFlat *tArray;
    CAknQueryValueTextArray *iValueTextArray;
    CAknQueryValueText *iTextValues;
};


enum data_view_form_type_e
{
    DVFT_NEW_ITEM,
    DVFT_EDIT_ITEM
};

class data_view_form_c : public CAknForm
{
public: // Constructor

	static data_view_form_c* NewL( database_manipulator_c *daddy, dbm_file_c *file, data_view_form_type_e type );
    void PrepareLC();

    bool dialing_mode()
    {
#ifdef __OS91__
		return false; //No sybian signed
#else
        return _dialing_field_id > 0 ? true : false;
#endif
    }

private: // from CAknForm

	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	TBool SaveFormDataL();

	void DoNotSaveFormDataL();

	void PreLayoutDynInitL();
    void PostLayoutDynInitL();

    void PrepareForFocusTransitionL ();
    void ProcessCommandL( TInt aCommandId ) ;

    TKeyResponse OfferKeyEventL  (  const TKeyEvent &    aKeyEvent, TEventCode    aType );


private: // Constructor

	data_view_form_c( database_manipulator_c *daddy, dbm_file_c *file , data_view_form_type_e type );
    ~data_view_form_c();

private: // members

	void LoadFormValuesFromDataL();

    void AddField( dbm_field_c *field );
    void SaveField( dbm_field_c *field );
    void SetFieldData( dbm_field_c *field );

    void DialNumberL(const TDesC& aPhoneNumber);

private: //data
    dbm_file_c *file;
    data_view_form_type_e type;

    RArray <data_view_form_list_item_s> list_items;
    database_manipulator_c *daddy;

    db_data_item_c stunt_item;
    int _dialing_field_id;
};


#endif
