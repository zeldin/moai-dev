#include <dfhack/MOAIFmodEventInstance.h>

//----------------------------------------------------------------//
int	MOAIFmodEventInstance::_isValid ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIFmodEventInstance, "U" )

    bool bReturnVal = false;

  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);

    lua_pushboolean( L, bReturnVal );

    return 1;
}

//----------------------------------------------------------------//
int	MOAIFmodEventInstance::_stop ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIFmodEventInstance, "U" )

  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);

	return 0;
}

//----------------------------------------------------------------//
int MOAIFmodEventInstance::_setVolume ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIFmodEventInstance, "UN" )

  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);

	return 0;
}


//================================================================//
// MOAIFmodEventInstance
//================================================================//

//----------------------------------------------------------------//
MOAIFmodEventInstance::MOAIFmodEventInstance () {
    RTTI_BEGIN
        RTTI_EXTEND ( MOAITransform )
    RTTI_END
}

//----------------------------------------------------------------//
MOAIFmodEventInstance::~MOAIFmodEventInstance () {
}

//----------------------------------------------------------------//
void MOAIFmodEventInstance::RegisterLuaClass ( MOAILuaState& state ) {

    MOAITransform::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAIFmodEventInstance::RegisterLuaFuncs ( MOAILuaState& state ) {

    MOAITransform::RegisterLuaFuncs ( state );

	luaL_Reg regTable [] = {
        { "isValid",			    _isValid },

		{ "stop",			        _stop },
		{ "setVolume",              _setVolume },

		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
