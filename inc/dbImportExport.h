/*------------------------------------------------------------------------------

                    Class DB Import / Export


    JCR 3.2003
------------------------------------------------------------------------------*/
#ifndef DB_IMPORT_EXPORT_H
#define DB_IMPORT_EXPORT_H

#include "dbMaster.h"
#include "dbDataItem.h"

#include <e32std.h>
#include <f32file.h>

class csv_reader_c;

#ifndef _NO_INTERFACE_PLEASE_
#include <aknprogressdialog.h>
#endif

class db_import_export_c : public MProgressDialogCallback
{
        csv_reader_c *pcsv;
        db_data_item_c db_item;

        dbm_file_c *file;

        HBufC* strTextError;
        HBufC* strTextInvalidFormat;

        RArray <TChar> separators;
        int current_separator;

        RArray <TDbBookmark> undo_bookmarks;

        bool write_str_to_rfile( RFile &file, TPtrC str , bool with_commas = true );
        bool write_eol_to_rfile( RFile &file );
        bool write_separator_to_rfile( RFile &file );

    public:
        db_import_export_c( dbm_file_c *file = 0);
        ~db_import_export_c();

        void set_file( dbm_file_c *_file )
        {
            file = _file;
        }

        bool do_export_csv( TPtrC path );
        bool do_import_csv( TPtrC path );
        bool do_import_csv_ex( TPtrC path );
        bool do_import_structure( TPtrC path );

        bool possible_undo()
        {
            return (undo_bookmarks.Count() ? true : false);
        }

        bool undo_import();

#ifndef _NO_INTERFACE_PLEASE_
        bool do_export_csv();
        bool do_import_csv();
        TChar select_separator();
        bool do_import_structure();

        CAknProgressDialog *iProgressDialog;

        virtual void DialogDismissedL (TInt aButtonId);
#endif
};
#endif
