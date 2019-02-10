/*-------------------------------------------------------------------------------

                SYMBIAN - DB master class
            Handles Database related issues
            Mantains Relationship with Symbian DB

- dbm_master_c Handles a database.
Each Database is stored in a multi-table file DBNAME.DB & a description
file called DBNAME.CDB

- dbm_file_c handles a table in the database

- dbm_field handles a column in a table

JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef DBMASTER_H
#define DBMASTER_H

#include <e32std.h>
#include <f32file.h>
#include <d32dbms.h>
#include <s32file.h>
#include <badesca.h>

#include "genericcfg.h"

#define FNAME_SIZE (256)
#define FIELD_SIZE (64)

class dbm_master_c;

enum dbm_field_type_e
{
    DFT_VOID,
    DFT_INT,
    DFT_TEXT,
    DFT_DATE,
    DFT_TIME,
    DFT_PHONE_NUMBER,
    DFT_FLOAT,
    DFT_LIST,
    DFT_YES_NO,
    DFT_BIGNUMBER,
    DFT_DATE_TIME,
    DFT_TEXT_NUMBER
};

enum dbm_field_key_type_e
{
    DFK_NO = 0,
    DFT_NORMAL,
    DFT_UNIQUE
};

class dbm_file_c;
class dbm_field_c;

struct link_details_s
{
    int file_id;
    int field_id;
    int field_id_to_show;

    dbm_file_c  *file;
    dbm_field_c *field;
    dbm_field_c *field_to_show;

    bool status;

    link_details_s()
    {
        file_id = 0;
        field_id = 0;
        field_id_to_show = 0;

        file = 0;
        field = 0;
        field_to_show = 0;

        status = false;
    }
};

class dbm_field_c
{
    friend class dbm_file_c;

    TBufC <FIELD_SIZE> _name;
    CDesCArrayFlat *pfield_text;

    int _id;
    int _max_len;
    dbm_field_type_e _type;
    int _extra;
    dbm_field_key_type_e _is_key;

    int col_idx_in_table;

    int _is_hidden;

    link_details_s link_details;

    public:
    dbm_field_c( int id ,TBufC <FIELD_SIZE> &name , dbm_field_type_e _type = DFT_TEXT , int max_len = KDbDefaultTextColLength );
    ~dbm_field_c();

    TBufC <FIELD_SIZE> &name()
    {
        return _name;
    }

    void set_name( const TDesC &new_name )
    {
        _name = new_name;
    }

    int id()
    {
        return _id;
    }

    dbm_field_type_e type()
    {
        return _type;
    }

    dbm_field_type_e visual_type()
    {
        return ( (_type == DFT_LIST || DFT_YES_NO ) ? DFT_TEXT : _type);
    }

    void set_type( dbm_field_type_e ntype )
    {
        _type = ntype;
    }

    int max_len()
    {
        return _max_len;
    }

    void set_max_len( int ml)
    {
        _max_len = ml;
    }

    int extra_number()
    {
        return _extra;
    }

    void set_extra_number( int e )
    {
        _extra = e;
    }

    TDbColType db_type();

    int get_n_extra_descs()
    {
        return ( pfield_text ? pfield_text->Count() : 0 );
    }

    TPtrC get_extra_desc( int idx )
    {
        TPtrC null;
        return ( pfield_text ? (*pfield_text) [idx] : null );
    }

    void add_extra_desc( const TDesC &new_name );

    void reset_extra_descs()
    {
        delete pfield_text;
        pfield_text = 0;
    }

    int get_col_idx_in_table()
    {
        return col_idx_in_table;
    }

    bool is_key()
    {
        return ( (_is_key == DFK_NO ) ? false : true );
    }

    dbm_field_key_type_e get_key_type()
    {
        return _is_key;
    }

    void set_key_type( dbm_field_key_type_e ktype )
    {
        _is_key = ktype;
    }

    bool is_hidden()
    {
        return _is_hidden;
    }

    void set_is_hidden( int val )
    {
        _is_hidden = val;
    }

    void link_to( int file_id, int file_id_field, int file_id_field_to_show );

    bool is_link()
    {
        return (link_details.file_id == 0 ? false : true );
    }

    link_details_s *get_link_details()
    {
        return &link_details;
    }
};

class dbm_master_c;

class dbm_file_c
{
    friend class dbm_master_c;

    RArray <dbm_field_c *> fields;

    TBufC <FIELD_SIZE>  _name;
    int _id;

    dbm_master_c *daddy;

    RDbTable table;
    CDbColSet * colSet;

    bool is_open;

    RArray <dbm_file_c *> to_close;

    bool handle_link_field( dbm_field_c *f );

    public:
    dbm_file_c(  int id ,TBufC <FIELD_SIZE> name );
    ~dbm_file_c();

    bool init( dbm_master_c *daddy );
    bool verify_tables();

    TBufC <FIELD_SIZE> &name()
    {
        return _name;
    }

    int id()
    {
        return _id;
    }

    int n_fields()
    {
        return fields.Count();
    }

    dbm_field_c *get_field( int idx )
    {
        return fields[ idx ];
    }

    dbm_field_c *find_field( int id );
    dbm_field_c *find_field( TPtrC name );

    int insert_field( dbm_field_c *afield );

    dbm_field_c *insert_field_ex( int id ,TBufC <FIELD_SIZE> name , dbm_field_type_e _type = DFT_TEXT , int max_len = KDbDefaultTextColLength );
    dbm_field_c *insert_field_kex( int id ,TBufC <FIELD_SIZE> name , dbm_field_type_e _type = DFT_TEXT , int max_len = KDbDefaultTextColLength, dbm_field_key_type_e ktype = DFT_NORMAL );
//    dbm_field_c *insert_field_ex( int id ,TPtrC name , dbm_field_type_e _type = DFT_TEXT , int max_len = KDbDefaultTextColLength );

    bool delete_field( int idx );

    // Call before using ...
    bool open( bool read_only = false );
    void close();

    RDbTable &get_table()
    {
        return table;
    }

    bool valid_field_name( TPtrC name );

    int get_next_empty_field_id();

    bool has_data();

    bool get_is_open()
    {
        return is_open;
    }

    bool sync_links();

};

class dbm_master_c : public generic_cfg_base_c
{
        TBufC <FNAME_SIZE> _path;
        TBufC <FNAME_SIZE> _name;
        
        TFileName _privatePath;

        RArray <dbm_file_c *> files;

        RFs fsSession;

        // DB Related
        CFileStore* store;
        RDbStoreDatabase database;

        bool init_tables();
        bool init_files();

        bool is_init;

    public:
        dbm_master_c( TBufC <FNAME_SIZE> &_path ,  TBufC <FNAME_SIZE> &_name );
        virtual ~dbm_master_c();

        bool init();

        dbm_file_c *get_file( int idx );
        int n_files();

        dbm_file_c *find_file( int id );
        dbm_file_c *find_file( TPtrC name );

        int insert_file( dbm_file_c * );

        RFs &Session()
        {
            return fsSession;
        }

        RDbStoreDatabase &Database()
        {
            return database;
        }

        TBufC <FNAME_SIZE> &path()
        {
            return _path;
        }

        TBufC <FNAME_SIZE> &name()
        {
            return _name;
        }

        bool save_defs();

    	void  ExternalizeL(RWriteStream& aStream) const;
	    void  InternalizeL(RReadStream& aStream);

        bool delete_all();
        bool delete_file( int idx );

        bool valid_file_name( TPtrC name );

};

/*
Rules for DBMS names
Tables, indexes and columns are named objects within a database. A DBMS name must begin with an alphabetic character, after which any alphabetic, numeric or the _ (underscore) character may be used. DBMS names are also limited to 64 characters in length.

Table names must be unique within a database, and columns and indexes must have unique names within the table to which they belong. For the purposes of uniqueness and identification, the names are folded before comparison, so two columns named column_one and Column_ONE are considered to have the same name.

Names in a database have no further restrictions.
*/
bool dbm_master_valid_name( TPtrC name );

// CHanges tabs to spaces to allow them to be correctly displayed.
bool dbm_master_clean_tabs( TDes &name );

void dbm_master_error( TPtrC desc, int id );

#ifdef _DBMASTER_EXTENSIONS_

#define DM_MASTER_CURRENT_CFG_VERSION (3) // 2=26.5.2004, 3=21.6.2004

_LIT( K_db_extension,".DB");
_LIT( K_cfg_extension,".CDB");
_LIT( K_cfg_identifier,"[QUODDB CFG FILE]");

#endif

#endif
