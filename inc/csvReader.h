/*-------------------------------------------------------------------------------


            Csv_reader_c
            Generic class that reads CSV files on Symbian


            NOTA: Falta preparar para separadores diferentes de ; !

            JCR 3.2004
------------------------------------------------------------------------------*/
#ifndef CSV_READER_H
#define CSV_READER_H

#include <e32std.h>
#include <f32file.h>
#include <s32std.h>
#include <s32file.h>


#define CMAX_FIELD_SIZE (512)
#define CMAX_LINE_SIZE (4016)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class csv_reader_line_info_c
{
        TBuf<CMAX_FIELD_SIZE> desc;
        TBuf<CMAX_FIELD_SIZE> text;
        TChar type_info;


    public:
        csv_reader_line_info_c()
        {
            type_info = TChar('T'); //TEXT!
        }

        void set_text( TBuf<CMAX_FIELD_SIZE> &_text )
        {
            text = _text;
        }

        void set_desc( TBuf<CMAX_FIELD_SIZE> &_desc )
        {
            desc = _desc;
        }

        void set_type_info( TChar type )
        {
            type_info = type;
        }

        void append_to_text( TChar &ch )
        {
            text.Append( ch );
        }

        void append_to_desc( TChar &ch )
        {
            desc.Append( ch );
        }


        TBuf<CMAX_FIELD_SIZE> get_desc()
        {
            return desc;
        }

        TBuf<CMAX_FIELD_SIZE> get_text()
        {
            return text;
        }

        TChar get_type_info()
        {
            return type_info;
        }

        void reset_text()
        {
            text.Zero();
        }

        void done( int idx );
};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class csv_reader_c
{
        RArray <csv_reader_line_info_c *> fields;

        bool needs_tobe_ended;

        RFs fsSession;
        TParse	filestorename;
        RFile file;

        TChar separator;

        TInt unicode;

        TBuf <CMAX_LINE_SIZE> line_buffer;

        bool file_line_to_buffer();

        bool file_line_to_buffer_normal();
        bool file_line_to_buffer_unicode();

        bool parse_line_buffer( bool header );
        bool read_header();
        bool get_smart_on_separators();

    public:
        csv_reader_c();
        csv_reader_c( TChar separator );

        ~csv_reader_c();

        bool start( const TDesC &aFilePath );
        bool goto_next_line();
        bool end();

        int n_fields()
        {
            return fields.Count();
        }

        csv_reader_line_info_c *get_field( int idx )
        {
            return fields[ idx ];
        }

        csv_reader_line_info_c *find_field( const TDesC &FieldName );

};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif
