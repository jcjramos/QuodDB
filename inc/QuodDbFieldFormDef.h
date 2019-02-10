/*------------------------------------------------------------------------------

                        QuodDB
                    Field Defenition Form


    JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef QUODDBFIELDFORMDEF_H
#define QUODDBFIELDFORMDEF_H

// INCLUDES

// System includes
#include <aknform.h> // CAknForm

class dbm_field_c;
class dbm_file_c;

class CAknQueryValueTextArray;
class CAknQueryValueText;


class CQuodDbFieldFormDef : public CAknForm
{
public: // Constructor

	static CQuodDbFieldFormDef* NewL( dbm_file_c *file, dbm_field_c *field );

private: // from CAknForm

	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	TBool SaveFormDataL();

	void DoNotSaveFormDataL();

	void PreLayoutDynInitL();

    void PrepareForFocusTransitionL ();
    void ProcessCommandL( TInt aCommandId ) ;

private: // Constructor

	CQuodDbFieldFormDef( dbm_file_c *file, dbm_field_c *field );
    ~CQuodDbFieldFormDef();

private: // members

	void LoadFormValuesFromDataL();

    void AddItemToList( TPtrC item );
    void ItemListChanged();

private: //data
    dbm_file_c *file;
    dbm_field_c *field;

    int current_type;
    CDesCArrayFlat *tYesNoArray;
    CDesCArrayFlat *tArray;
    CDesCArrayFlat *tEmptyArray;
    CAknQueryValueTextArray *iValueTextArray;
    CAknQueryValueText *iTextValues;

    bool in_list_hidden_status;
};


#endif
