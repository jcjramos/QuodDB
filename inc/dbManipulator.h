/*------------------------------------------------------------------------------

                        Database Manipulator

                Low Level Access to functions to a Database
    JCR 4.2004
------------------------------------------------------------------------------*/
#ifndef DBMANIPULATOR_H
#define DBMANIPULATOR_H

#include "dbMaster.h"
#include "dbDataItem.h"
#include "dbImportExport.h"
#include "genericCfg.h"

class database_manipulator_c: public generic_cfg_base_c
{
protected:
    dbm_master_c *database;
    dbm_file_c *file;

    dbm_field_c *heading;
    dbm_field_c *showing;

    bool use_heading;
    int heading_field_idx;

    RArray <dbm_field_c *> visible_columns;
    RArray <db_filter_item_c *> filters;

    RArray <TDbBookmark> bookmarks;

    int number_of_keys;
    int last_sort;

    db_data_item_c data_item;

    TBuf <1024> line;
    TBuf <1024> stunt;

    db_import_export_c import_export;

    bool editable;
    bool use_filters;
    bool is_ok;

    bool store_preferences;

    bool is_visible( dbm_field_c *field );
    void append_coldata_to_line( dbm_field_c *field );

    bool sort_by( int sort_field_idx );
    // Load & Dave selections
    bool save();
    bool load();

	virtual void ExternalizeL(RWriteStream& aStream) const;
    virtual void InternalizeL(RReadStream& aStream);

public:
    database_manipulator_c();
    ~database_manipulator_c();

    bool prepare_for_db( dbm_master_c *database, dbm_file_c *file );

    void reset();

    dbm_master_c *get_database()
    {
        return database;
    }

    dbm_file_c *get_file()
    {
        return file;
    }

    bool live( bool editable , bool use_filters );
    bool die();

    CDesCArrayFlat *reload_as_array( int active_visible_column = 0 );

    bool get_is_editable()
    {
        return editable;
    }

    bool get_use_heading()
    {
        return use_heading;
    }

    int get_number_of_keys()
    {
        return number_of_keys;
    }

    db_import_export_c &get_import_export()
    {
        return import_export;
    }

    int n_visible_columns()
    {
        return visible_columns.Count();
    }

    dbm_field_c *get_visible_column( int idx )
    {
        return visible_columns[idx];
    }

    void reset_filters();
    
    int n_filters()
    {
        return filters.Count();
    }

    db_filter_item_c *get_filter( int idx )
    {
        return filters[idx];
    }

    int add_filter( db_filter_item_c *ff )
    {
        filters.Append( ff );
        return n_filters();
    }

    int n_records()
    {
        return bookmarks.Count();
    }

    bool select_record( int idx );

    void set_store_preferences( bool yes_or_no )
    {
        store_preferences = yes_or_no;
    }

    void set_header_field( int heading_idx  );

};

class database_manipulator_ui_c: public database_manipulator_c
{
    public:
      void add_line();
      void delete_line( int idx );
      void edit_line( int idx );

      bool select_header();
      bool select_sort();
      bool select_pages();
      bool select_filters_add();
      bool select_filters_remove();
      int select_lookup();
};

#endif
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

