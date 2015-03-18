#ifndef	MOAIFMODMICROPHONE_H
#define	MOAIFMODMICROPHONE_H

#include <moai-sim/headers.h>

//================================================================//
// MOAIFmodMicrophone
//================================================================//

class MOAIFmodMicrophone :
	public MOAITransform {
private:

	//----------------------------------------------------------------//

    static int _setLoc		( lua_State* L );

public:

    DECL_LUA_FACTORY ( MOAIFmodMicrophone )
    DECL_ATTR_HELPER ( MOAIFmodMicrophone )

	//----------------------------------------------------------------//
                MOAIFmodMicrophone	();
	virtual		~MOAIFmodMicrophone	();

	void		RegisterLuaClass		( MOAILuaState& state );
	void		RegisterLuaFuncs		( MOAILuaState& state );
};

#endif
