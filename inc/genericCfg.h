/*

                Database Master Config File
                Keeps a Database Description

    JCR 3.2004
*/
#ifndef GENERICCFG_H
#define GENERICCFG_H

#include <e32std.h>
#include <s32file.h>
#include <e32cons.h>
#include <f32file.h>

// GLOBAL HELPER FUNCTIONS, Writes NUMBER OF BYTES + STR !
#define GC_READ_STR(str) {int len = aStream.ReadInt32L();aStream.ReadL( str, len );};
#define GC_WRITE_STR(str) {aStream.WriteInt32L( str.Length());aStream.WriteL( str );};


// User classes must inherit fromt this base ...
class generic_cfg_base_c : public CBase
{
 public:
	virtual void ExternalizeL(RWriteStream& aStream) const
    {

    }

    virtual void InternalizeL(RReadStream& aStream)
    {

    }

    void set_default();
};

class generic_cfg_c
{
    generic_cfg_base_c *object;

    bool _load( TPtrC  path, TPtrC  file, TPtrC  extension );
    bool _save( TPtrC  path, TPtrC  file, TPtrC  extension );

public:
    generic_cfg_c( generic_cfg_base_c *object );
    ~generic_cfg_c();

    bool load( TPtrC  path, TPtrC  file, TPtrC  extension );
    bool save( TPtrC  path, TPtrC  file, TPtrC  extension );
};

#endif
