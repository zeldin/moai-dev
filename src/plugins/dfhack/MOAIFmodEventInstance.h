#ifndef	MOAIFMODEVENTINSTANCE_H
#define	MOAIFMODEVENTINSTANCE_H

#include <moai-sim/headers.h>

//================================================================//
// MOAIFmodEventInstance
//================================================================//
class MOAIFmodEventInstance :
	public MOAITransform {
private:

	//----------------------------------------------------------------//
    static int _isValid                ( lua_State* L );

    static int _stop                   ( lua_State* L );
    static int _setVolume			    ( lua_State* L );
    static int	_setParameter		    ( lua_State* L );

public:

	DECL_LUA_FACTORY ( MOAIFmodEventInstance )
    DECL_ATTR_HELPER ( MOAIFmodEventInstance )

	//----------------------------------------------------------------//
				MOAIFmodEventInstance	();
	virtual		~MOAIFmodEventInstance	();

	void		RegisterLuaClass		( MOAILuaState& state );
	void		RegisterLuaFuncs		( MOAILuaState& state );
};

#endif
