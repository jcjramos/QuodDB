/*-------------------------------------------------------------------------------


            Csv_reader_c
            Generic class that reads CSV files on Symbian


JCR 3.2004
------------------------------------------------------------------------------*/

#include "csvreader.h"
#include <eikenv.h>


_LIT( kTypeId, "[TYPE=");

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

csv_reader_c::csv_reader_c()
{
    separator = TChar( ',' );

    needs_tobe_ended = false;
}

csv_reader_c::csv_reader_c( TChar sep )
{
    separator = sep;
    needs_tobe_ended = false;
}

csv_reader_c::~csv_reader_c()
{
    end();

    for( int n = 0; n < fields.Count(); n++ )
    {
        delete fields[n];
    }

    fields.Close();
}

bool csv_reader_c::end()
{
    if( !needs_tobe_ended )
            return ( false );


    file.Close();


    fsSession.Close(); //close the file session

    needs_tobe_ended = false;

    return ( true );
}

bool csv_reader_c::start( const TDesC &file_path )
{
	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return false;
    }

    unicode = false;

    TTime dummy;

    if( fsSession.Modified( file_path, dummy ) != KErrNone ) // File does not exist !
            return ( false );

    // Open the direct file store
	fsSession.Parse( file_path , filestorename );

    if( file.Open( fsSession, filestorename.FullName(),EFileRead | EFileStreamText ) != KErrNone )
            return ( false );

    read_header();
//	instream  >> *pcfg;

    needs_tobe_ended = true;

    return ( true );
}

bool csv_reader_c::read_header()
{
    file_line_to_buffer();

    if( line_buffer[0] == 0xff && line_buffer[1] == 0xfe ) // Unicode !
    {
        unicode = 1;
        int n = 2;
        file. Seek( ESeekStart , n );
        file_line_to_buffer();
    }

    if( line_buffer[0] == 0xfe && line_buffer[1] == 0xff ) // Reversed Unicode !
    {
        unicode = 2;
        int n = 2;
        file. Seek( ESeekStart , n );
        file_line_to_buffer();
    }

    // LOOKS LIKE UNICODE ?
    if( line_buffer.Length() > 10 && line_buffer[1] == 0 && line_buffer[3] == 0 && line_buffer[5] == 0 )
    {
          unicode = 1;
          int n = 2;
          file. Seek( ESeekStart , n );
          file_line_to_buffer();
    }

    // LOOKS LIKE REVERSED UNICODE ?
    if( line_buffer.Length() > 10 && line_buffer[0] == 0 && line_buffer[2] == 0 && line_buffer[4] == 0 )
    {
          unicode = 2;
          int n = 2;
          file. Seek( ESeekStart , n );
          file_line_to_buffer();
    }

    get_smart_on_separators();

    parse_line_buffer( true );

    return ( true );
}

bool csv_reader_c::goto_next_line()
{
    if( file_line_to_buffer() )
    {
        parse_line_buffer( false );

        return ( true );
    }
    else
    {
        return ( false );
    }
}

bool csv_reader_c::file_line_to_buffer_normal()
{
   line_buffer.FillZ();
   line_buffer.Zero();


   TBuf8 <2> buff;
   TBuf <2> masta;

	bool incommas = false;
	
   while( file.Read( buff, 1 ) == KErrNone )
   {
      if( !buff.Length() )
              break;
      
      
      if( buff[0] == '"' )
      {
      		incommas = !incommas;
      }

		// New lines are allowed bettween commas
      if( !incommas && (buff[0] == '\n' || buff[0] == '\r' ))
      {
          file.Read( buff, 1 ); // Read the other line jump caracter and continue ...

          return ( true );
      }

      masta.Copy( buff ); // Handles the convertion between 8 vs 16 bit strings !
      line_buffer.Append( masta );
   }

   return ( false );
}

bool csv_reader_c::file_line_to_buffer_unicode()
{
    line_buffer.FillZ();
     line_buffer.Zero();


     TBuf8 <3> buff;
     TBuf8 <3> reva;
     TBuf <3> masta;

	 TBool incommas = false;
	 
     while( file.Read( buff, 2 ) == KErrNone )
     {
        if( !buff.Length() )
                break;

        if( unicode == 2 )
        {
            reva.Copy( buff );
            reva[0] = buff[1];
            reva[1] = buff[0];

            masta.Copy( (TUint16 *) reva.PtrZ(), 1 );
        }
        else
        {
            masta.Copy( (TUint16 *) buff.PtrZ(), 1 );
        }

		if( masta[0] == TChar('"' ))
		{
			incommas = !incommas;
		}

		// New lines are allowed bettween commas
        if( !incommas && masta[0] == TChar('\n') || masta[0] == TChar('\r')  )
        {
            file.Read( buff, 2 ); // Read the other line jump caracter and continue ...
            return ( true );
        }

        //masta.Copy( buff ); // Handles the convertion between 8 vs 16 bit strings !
        line_buffer.Append( masta );
     }

     return ( false );
}

bool csv_reader_c::file_line_to_buffer()
{
    if( unicode )
    {
        return file_line_to_buffer_unicode();
    }
    else
    {
        return file_line_to_buffer_normal();
    }

}

bool csv_reader_c::parse_line_buffer( bool header )
{

    TChar string_start( '"');
    TChar fist_good_char( ' ');
//    TChar separator( ';');

    int current_field_idx = 0;
    csv_reader_line_info_c *current_field;

    if( header )
    {
        current_field = new csv_reader_line_info_c;
        fields.Append( current_field );
    }
    else
    {
        current_field = fields[ current_field_idx ];
        current_field->reset_text();
    }

    for( int n = 0; n < line_buffer.Length(); n++ )
    {
        TChar current = line_buffer[n];

        if( current == separator )
        {
            current_field_idx++;

            if( header )
            {
                if( current_field )
                    current_field->done(fields.Count());

                current_field = new csv_reader_line_info_c;
                fields.Append( current_field );
            }
            else
            {
                current_field = fields[ current_field_idx ];
                current_field->reset_text();
            }

            continue;
         }

       if( current == string_start ) // Walk until string end ! may contain commas
       {
            n++;
            current = line_buffer[n];

            while( current != string_start )
            {

                if( header )
                {
                        current.UpperCase();
                        current_field->append_to_desc( current );
                }
                else
                {
                        current_field->append_to_text( current );
                }

                n++;
                current = line_buffer[n];
            }

            continue;
       }


       if( current < fist_good_char )
            continue;

        if( header )
        {
                current.UpperCase();
                current_field->append_to_desc( current );
        }
        else
        {
                current_field->append_to_text( current );
        }

    }

    if( current_field )
            current_field->done(fields.Count());

    return ( true );
}


void csv_reader_line_info_c::done( int idx ) // Check if title contains type info !
{
    int typepos = desc.Find( kTypeId );

    if( typepos != KErrNotFound )
    {
            type_info = desc[typepos+6];

            desc.SetLength( typepos -1 );
    }
    
    //Parse the desc to remove wrong chars
    for( int p = 0; p < desc.Length(); p++ )
    {        	
    	TChar ch = desc[p];
    	
    	
    	if( !ch.IsAlpha() )
    	{
    		// if it's the last we remove
    		if( p > 1 && p == desc.Length()-1 )
    		{
    			desc.SetLength( p );
    		}
    		else //else we replace
    		{
    			desc[p]	= TChar('_');	
    		}    		
    	}
    }
        
        
    if( desc.Length() == 0 )
    {
    	desc.Copy(_L("COL_"));
    	desc.AppendNum( idx );    	
    }
    
}

csv_reader_line_info_c *csv_reader_c::find_field( const TDesC &name )
{
    TBuf <255> tname( name );
    tname.UpperCase();

    for( int n = 0; n < fields.Count(); n++ )
    {
        if( fields[n]->get_desc() == tname )
        {
            return ( fields[n] );
        }
    }

    return ( 0 );
}

// We check the first line to see if we can find the selected separator,
// if not we try other one !
bool csv_reader_c::get_smart_on_separators()
{
    #define VALID_SEPARATORS (3)

    int n_found[VALID_SEPARATORS];
    TChar seps[VALID_SEPARATORS];
    seps[0]=',';
    seps[1]=';';
    seps[2]='\t';
    int current_sep = -1;

    for( int a = 0; a < VALID_SEPARATORS; a++ )
    {
        n_found[a] = 0;

        if( seps[a] == separator )
                current_sep = a;
    }

    if( current_sep < 0 || line_buffer.Length() == 0 ) //UNK Separator or no data!
            return ( false );

    for( int n = 0; n < line_buffer.Length(); n++ )
    {
        for( int a = 0; a < VALID_SEPARATORS; a++ )
        {
            if( line_buffer[n] == seps[a] )
                n_found[a]++;
        }
    }

    if( n_found[current_sep] == 0 ) // NOT FOUND ! THAT's SUSPECT
    {
        for( int a = 0; a < VALID_SEPARATORS; a++ )
        {
            if( n_found[a] )
            {
                separator = seps[a];   // Change to a valid separator
                break;
            }
        }

    }

    return ( true );
}


