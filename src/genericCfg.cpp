/*

                Database Master Config File
                Keeps a Database Description

    JCR 3.2004
*/
#include "genericcfg.h"

#include <eikenv.h>
#include <e32std.h>
#include <f32file.h>
#include <f32file.h>
#include <s32std.h>
#include <s32file.h>


#define FNAME_SIZE (256)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

generic_cfg_c::generic_cfg_c( generic_cfg_base_c *_object )
{
    object = _object;
}

generic_cfg_c::~generic_cfg_c()
{

}

bool generic_cfg_c::load( TPtrC  path, TPtrC  file, TPtrC  extension )
{
    bool ret = false;
    ret = _load( path, file, extension );
    return ret;
}

bool generic_cfg_c::save( TPtrC  path, TPtrC  file, TPtrC  extension )
{
    bool ret = false;
    ret = _save( path, file, extension );
    return ret;
}

bool generic_cfg_c::_load(  TPtrC  path, TPtrC  file, TPtrC  extension )
{

    RFs fsSession;

	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return false;
    }

    TBuf <FNAME_SIZE> file_path( path );
    // Append the file name & Extension
    file_path.Append( file );
    file_path.Append( extension );

    TTime dummy;

    if( fsSession.Modified( file_path, dummy ) != KErrNone ) // File does not exist !
    {
            fsSession.Close(); //close the file session
            return ( false );
    }

//    CEikonEnv::Static()->AlertWin( file_path , extension );

    // Open the direct file store
	TParse	filestorename;
	fsSession.Parse( file_path , filestorename );
	CFileStore* store = CDirectFileStore::OpenL(fsSession,filestorename.FullName(),EFileRead);

	RStoreReadStream instream;
	instream.OpenL(*store,store->Root());

    int x = 0;
    TRAP( x , instream  >> *object );

    if( x )
    {
        CEikonEnv::Static()->AlertWin( _L("Fatal Error ! cannot load"), filestorename.FullName() );
        User::Leave( x );
    }

    instream.Release();

	delete store;

    fsSession.Close(); //close the file session

    return ( true );

}

bool generic_cfg_c::_save( TPtrC  path, TPtrC  file, TPtrC  extension )
{
    RFs fsSession;
    TParse	filestorename;

	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return false;
    }

    TBuf <FNAME_SIZE> file_path( path );

    // Make sure the path exists
    fsSession.MkDirAll( file_path );
    // Append the file name & Extension
    file_path.Append( file );
    file_path.Append( extension );

//    CEikonEnv::Static()->AlertWin( _L("Save cfgs"), file_path );


	fsSession.Parse( file_path, filestorename );

	CFileStore * store = CDirectFileStore::ReplaceL(fsSession,filestorename.FullName(),EFileWrite);

    store->SetTypeL( TUidType(store->Layout()) );

    // Construct the output stream.
	RStoreWriteStream outstream;

	TStreamId id = outstream.CreateL(*store);

	outstream  << *object;

	outstream.CommitL();

    // Set this stream id as the root
	store->SetRootL(id);

    // Commit changes to the store
	store->CommitL();

    outstream.Release();

	delete store;

    fsSession.Close(); //close the file session

    return ( true );
}


