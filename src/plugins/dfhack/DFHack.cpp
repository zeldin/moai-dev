#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>
#include <moai-sim/headers.h>

#include <dfhack/DFHack.h>

//----------------------------------------------------------------//

STLString DFHack::BuildLocalDocumentFilename( MOAILuaState &state, int idx )
{
  idx = state.AbsIndex(idx);
  cc8 *p = state.GetValue<cc8*>(idx, "");
  if (*p != '/') {
    MOAIEnvironment::Get ().PushLuaClassTable ( state );
    if ( state.GetFieldWithType ( -1, MOAI_ENV_documentDirectory,
				  LUA_TSTRING )) {
      cc8 *str = lua_tostring(state, -1);
      if (*str) {
	lua_pushstring(state, "/");
	lua_pushvalue(state, idx);
	lua_concat(state, 3);
	STLString s = lua_tostring(state, -1);
	state.Pop(2);
	return s;
      }
      state.Pop(2);
    } else
      state.Pop(1);
  }
  return p;
}

//----------------------------------------------------------------//

int DFHack::ftWalk(const char *fpath, const struct stat *sb, int typeflag)
{
  if (fpath[0] == '.' && fpath[1] == '/')
    fpath += 2;
  if (fpath[0] == '.' && fpath[1] == 0)
    return 0;
  if (typeflag == FTW_F) {
    MOAIScopedLuaState state = MOAILuaRuntime::Get ().State ();
    STLString lpath = fpath;
    lpath.to_lower();
    lua_pushstring(state, lpath);
    lua_pushstring(state, fpath);
    lua_settable(state, -3);
  }
  return 0;
}

//----------------------------------------------------------------//

int DFHack::_loadFromInstructionStream ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "S" )) return 0;
  STLString path = BuildLocalDocumentFilename(state, 1);
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
    MOAILog ( NULL, MOAILogMessages::MOAI_FileOpenError_S, path.str() );
  }

  return numResults;
}

//----------------------------------------------------------------//

int DFHack::_saveInstructionStream ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "STTNB" )) return 0;
  STLString path = BuildLocalDocumentFilename(state, 1);
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
    MOAILog ( NULL, MOAILogMessages::MOAI_FileOpenError_S, path.str() );
  }

  return 0;
}

//----------------------------------------------------------------//

int DFHack::_getStorageListing ( lua_State* L )
{
  MOAILuaState state ( L );
  lua_pushstring(state, "");
  lua_newtable(state);
  STLString oldCwd = ZLFileSys::GetCurrentPath();
  ZLFileSys::SetCurrentPath(BuildLocalDocumentFilename(state, -2));
  ftw(".", ftWalk, 16);
  ZLFileSys::SetCurrentPath(oldCwd);
  return 1;
}

//----------------------------------------------------------------//

int DFHack::_beginSubscriptionUpdate ( lua_State* L )
{
  MOAILuaState state ( L );
  lua_pushnil(state);
  return 1;
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
    { "getStorageListing",  _getStorageListing },
    { "beginSubscriptionUpdate",  _beginSubscriptionUpdate },
    { NULL, NULL }
  };
  luaL_register ( state, 0, regTable );
}

