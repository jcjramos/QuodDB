/*-------------------------------------------------------------------------------



            QuodDB master class

JCR 3.2004
------------------------------------------------------------------------------*/

#define _DBMASTER_EXTENSIONS_
#include "dbmaster.h"
#include "quodDbMaster.h"

#include <f32file.h>
#include <d32dbms.h>
#include <s32file.h>
#include <eikenv.h>
#include <eikapp.h>
#include <aknappui.h>
//#include <eikonenvironment.h>


// ATT: Estranhamente "desenvolve" na directoria C:\Symbian\Series60_1_2_B\epoc32\winsb\c\System\apps\QuodDb e não no emulador !
//_LIT( K_App_Database_Path,"C:\\system\\apps\\QuodDB\\" ); deprecated !
_LIT( K_App_Database_folder,"Database\\" );
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

quod_db_master_c::quod_db_master_c(CAknViewAppUi *_app_ui):databases( 10 ),app_ui(_app_ui)
//,pata( K_App_Database_Path ) WAS LIKE THIS ...
{
    current_db = 0;
}

quod_db_master_c::~quod_db_master_c()
{
    delete current_db;

//    databases.Close();
}

bool quod_db_master_c::init()
{
    pata.Copy(  ApplicationDriveAndPath() );
    pata.Append( K_App_Database_folder );

    databases.Reset();

    RFs fsSession;

	TInt fsret = fsSession.Connect(); // start a file session
	if (fsret != KErrNone)
    {
        return false;
    }

    fsSession.MkDir( pata ); // Create the folder if doesn-t exist !

    TBuf <256> _pata( pata );
    _pata.Append( _L("*") );

    CDir* dirList;

    fsSession.GetDir( _pata , KEntryAttMaskSupported , ESortByName, dirList );

    if( dirList && dirList->Count() )
    {
        for (TInt i=0;i<dirList->Count();i++)
        {
            TBuf<FNAME_SIZE> ooname( (*dirList)[i].iName );
            ooname.UpperCase();

            if( ooname.Find( K_cfg_extension ) == KErrNotFound )
                    continue;

                // Take out the .XXX
            TBuf <FNAME_SIZE> stunt( (*dirList)[i].iName );
            stunt.SetLength( stunt.Length() - 4 );

            databases.AppendL( stunt );
        }

    }

    delete dirList;

    fsSession.Close(); //close the file session

    return ( true );
}

int quod_db_master_c::n_databases()
{
    return ( databases.Count() );
}

TPtrC quod_db_master_c::database_name( int idx )
{
    return ( databases[idx] );
}

dbm_master_c *quod_db_master_c::open_database( TPtrC name )
{
    for( int n = 0; n < n_databases(); n++ )
    {
        if( name == database_name( n ) )
        {
            return open_database( n );
        }
    }

    return ( 0 );
}

dbm_master_c *quod_db_master_c::open_database( int idx )
{
    // Must make a copy cause K_app is static !!!!
    TBufC <256> _pata( pata );
    TBufC <256> file( databases[idx] );

//    CEikonEnv::Static()->AlertWin( K_App_Database_Path, databases[idx] );
//    CEikonEnv::Static()->AlertWin( _pata, file );

    dbm_master_c *database = new dbm_master_c( _pata, file );

    if(!database)
            return ( NULL );

    database->init();

    return database;
}

dbm_master_c *quod_db_master_c::new_database( TPtrC name )
{
//    CEikonEnv::Static()->AlertWin( _L("File name"), name );

    databases.AppendL( name );

    return open_database( databases.Count() -1 );
}

bool quod_db_master_c::delete_database( TPtrC name )
{
    for( int n = 0; n < databases.Count(); n++ )
    {
        if( databases[n] == name )
        {
            return ( delete_database( n ) );
        }
    }

    return ( false );
}

bool quod_db_master_c::delete_database( int idx )
{
    // Make sure to update the current database
    if( current_db && databases[idx] == current_db->name() )
    {
        delete current_db;
        current_db = 0;
        
        databases.Delete( idx );
    }

    // Must make a copy cause K_app is static !!!!
    TBufC <256> _pata( pata );
    TBufC <256> file( databases[idx] );

    dbm_master_c *db = new dbm_master_c( _pata, file );

    if( !db )
            return ( false );

    bool ret = db->delete_all();

    delete db;

    return ( ret );

}

bool quod_db_master_c::valid_db_name( TPtrC name )
{
     if( !dbm_master_valid_name( name ) )
            return ( false );

     TBuf <256> _name( name );
     _name.UpperCase();

     TBuf <256> stunt( name );

     // EXISTS ?
     for( int n = 0; n < n_databases(); n++ )
     {
        stunt.Zero();
        stunt.Append( database_name( n ) );
        stunt.UpperCase();

        if( stunt == _name )
                return ( false );
     }

     return ( true );
}

TFileName quod_db_master_c::ApplicationDriveAndPath()
{
#ifdef __OS91__
	RFs fs;
	fs.Connect();
#ifdef __WINS__	
	TFileName appfullname = _L("C:\\A");
#else
	TFileName appfullname = app_ui->Application()->AppFullName();
#endif	
	//CEikonEnv::Static()->InfoWinL( _L("AppPath"),privatePath);
	TFileName _privatePath;
	fs.PrivatePath( _privatePath );

	privatePath.Copy( appfullname.Left(2) );
	privatePath.Append( _privatePath );

	fs.Close();	
	return privatePath;	
#else
	TParse parse;
//On WINS the application is on the z drive
#ifdef __WINS__
	TFileName appfullname = app_ui->Application()->AppFullName();
	parse.Set(_L("c:"), &appfullname, NULL);
#else
	parse.Set(app_ui->Application()->AppFullName(), NULL, NULL);
#endif
	return parse.DriveAndPath();
#endif	
}

