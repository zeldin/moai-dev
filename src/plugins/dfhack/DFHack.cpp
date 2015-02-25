#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>

#include <dfhack/DFHack.h>

//----------------------------------------------------------------//

DFHack::DFHack ()
{
  RTTI_BEGIN
    RTTI_EXTEND ( MOAILuaObject )
  RTTI_END
}

//----------------------------------------------------------------//

DFHack::~DFHack ()
{
}

//----------------------------------------------------------------//

void DFHack::RegisterLuaClass ( MOAILuaState& state )
{
  luaL_Reg regTable [] = {
    { NULL, NULL }
  };
  luaL_register ( state, 0, regTable );
}

