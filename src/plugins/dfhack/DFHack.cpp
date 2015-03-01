#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>

#include <dfhack/DFHack.h>

//----------------------------------------------------------------//

int DFHack::_loadFromInstructionStream ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "S" )) return 0;
  cc8* path = state.GetValue < cc8* >( 1, "" );
  bool localOnly = state.GetValue < bool >( 2, false );

  ZLFileStream stream;
  int numResults = 0;
  if ( stream.OpenRead ( path )) {
    Lua51::DumpReader reader(stream);

    if (!reader.CheckHeader()) {
      MOAIPrint("bad header\n");
    } else {
      Lua51::Function func;
      if (func.undump(reader, state))
	numResults = func.interpret(state);
    }
    stream.Close ();
  } else {
    MOAILog ( NULL, MOAILogMessages::MOAI_FileOpenError_S, path );
  }

  return numResults;
}

//----------------------------------------------------------------//

int DFHack::_saveInstructionStream ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "STTNB" )) return 0;
  cc8* path = state.GetValue < cc8* >( 1, "" );
  int registerCount = state.GetValue < s16 >( 4, 0 );
  bool storeLocal = state.GetValue < bool >( 5, false );

  Lua51::Function func;
  if (!func.create(state, 2, 3, registerCount))
    return 0;

  ZLFileStream stream;
  if ( stream.OpenWrite ( path )) {
    Lua51::DumpWriter writer(stream);
    writer.WriteHeader();
    func.dump(writer, state);
    stream.Close ();
  } else {
    MOAILog ( NULL, MOAILogMessages::MOAI_FileOpenError_S, path );
  }

  return 0;
}

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
  lua_newtable(state);
  state.SetField(-1, "LOADK", Lua51::OP_LOADK);
  state.SetField(-1, "RETURN", Lua51::OP_RETURN);
  state.SetField(-1, "NEWTABLE", Lua51::OP_NEWTABLE);
  state.SetField(-1, "SETTABLE", Lua51::OP_SETTABLE);
  lua_newtable(state);
  state.SetField(-1, "iABC", Lua51::iABC);
  state.SetField(-1, "iABx", Lua51::iABx);
  lua_setfield(state, -2, "OpMode");
  lua_newtable(state);
  state.SetField(-1, "OpArgR", Lua51::OpArgR);
  lua_setfield(state, -2, "OpArgMask");
  lua_setfield(state, -2, "ops");

  luaL_Reg regTable [] = {
    { "loadFromInstructionStream",   _loadFromInstructionStream },
    { "saveInstructionStream",   _saveInstructionStream },
    { NULL, NULL }
  };
  luaL_register ( state, 0, regTable );
}

