#include <dfhack/MOAIFmodMicrophone.h>


//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//

int	MOAIFmodMicrophone::_setLoc ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//================================================================//
// MOAIFmodMicrophone
//================================================================//

//----------------------------------------------------------------//
MOAIFmodMicrophone::MOAIFmodMicrophone () {
    RTTI_BEGIN
        RTTI_EXTEND ( MOAITransform )
    RTTI_END
}

//----------------------------------------------------------------//
MOAIFmodMicrophone::~MOAIFmodMicrophone () {
}

//----------------------------------------------------------------//
void MOAIFmodMicrophone::RegisterLuaClass ( MOAILuaState& state ) {

    MOAITransform::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAIFmodMicrophone::RegisterLuaFuncs ( MOAILuaState& state ) {

    MOAITransform::RegisterLuaFuncs ( state );

    luaL_Reg regTable [] = {
        { "new",            MOAILogMessages::_alertNewIsUnsupported },
	{ "setLoc",         _setLoc },
        { NULL, NULL }
    };

    luaL_register ( state, 0, regTable );
}

