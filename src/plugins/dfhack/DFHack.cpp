#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>
#include <moai-sim/headers.h>

#include <dfhack/DFHack.h>
#include <dfhack/Lua5Introspection.h>

#include <fts.h>

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

int DFHack::_hasData ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "S" )) return 0;
  STLString path = BuildLocalDocumentFilename(state, 1);
  bool localOnly = state.GetValue < bool >( 2, false );

  bool result = ZLFileSys::CheckFileExists ( path, true );

  lua_pushboolean ( state, result );

  return 1;
}

//----------------------------------------------------------------//

int DFHack::_deleteData ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "S" )) return 0;
  STLString path = BuildLocalDocumentFilename(state, 1);
  bool localOnly = state.GetValue < bool >( 2, false );

  bool result = ZLFileSys::DeleteFile ( path );

  lua_pushboolean ( state, result );

  return 1;
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
  if ( !state.CheckParams ( 1, "STTN" )) return 0;
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
  char path[] = { '.', 0 };
  char * const paths[] = { path,  NULL };
  FTS *fts = fts_open(paths, 0, NULL);
  FTSENT *ent;
  while ((ent = fts_read(fts)) != NULL) {
    if (ent->fts_info == FTS_F) {
      char *fpath = ent->fts_path;
      if (fpath[0] == '.' && fpath[1] == 0)
	fpath ++;
      else if (fpath[0] == '.' && fpath[1] == '/')
	fpath += 2;
      if (fpath[0] != 0) {
	STLString lpath = fpath;
	lpath.to_lower();
	lua_pushstring(state, lpath);
	lua_pushstring(state, fpath);
	lua_settable(state, -3);
      }
    }
  }
  fts_close(fts);
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

int DFHack::_setTimeDilation ( lua_State* L )
{
  MOAILuaState state ( L );

  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);

  return 0;
}

//----------------------------------------------------------------//

int DFHack::_registerThread ( lua_State* L )
{
  MOAILuaState state ( L );

  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);

  return 0;
}

//----------------------------------------------------------------//

int DFHack::_registerWrappedThread ( lua_State* L )
{
  MOAILuaState state ( L );

  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);

  return 0;
}

//----------------------------------------------------------------//

int DFHack::_getDisassembly ( lua_State* L )
{
  MOAILuaState state ( L );

  if (lua_type (L, 1) == LUA_TFUNCTION) {
#if MOAI_WITH_LUAJIT
    return luaL_error(L, "DFHack.getDisassembly currently not supported for LuaJIT");
#else
    return static_cast<int>(Lua5Introspection::PushDisassembly(state, static_cast<const Closure*>(lua_topointer(L, 1))));
#endif    
  } else {
    return luaL_argerror(L, 1, "function expected");
  }
}

//----------------------------------------------------------------//

int DFHack::_getAssetListing ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "S" )) return 0;
  cc8 *p = state.GetValue<cc8*>(1, "");
  fprintf(stderr, "getAssetListing(\"%s\")\n", p);
  STLString oldPath = ZLFileSys::GetCurrentPath();
  if ( !ZLFileSys::SetCurrentPath ( p )) {
    return 0;
  }

  ZLDirectoryItr dirItr;
  lua_newtable ( L );
  int n = 0;
  dirItr.Start ();
  while ( dirItr.NextDirectory ()) {
    if( strcmp(dirItr.Current(), "..") == 0 ||
	strcmp(dirItr.Current(), ".") == 0 ) {	
      continue;	
    }
    lua_pushstring ( L, dirItr.Current ());
    n++;
#ifdef luaL_setn
    luaL_setn ( L, -2, n );  // new size
#endif
    lua_rawseti ( L, -2, n );  // t[pos] = v
  }
  lua_newtable ( L );
  n = 0;
  dirItr.Start ();
  while ( dirItr.NextFile ()) {
    lua_pushstring ( L, dirItr.Current ());
    n++;
#ifdef luaL_setn
    luaL_setn ( L, -2, n );  // new size
#endif
    lua_rawseti ( L, -2, n );  // t[pos] = v
  }

  ZLFileSys::SetCurrentPath ( oldPath );
  return 2;
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
  state.SetField(-1, "iAsBx", Lua51::iAsBx);
  lua_setfield(state, -2, "OpMode");
  lua_newtable(state);
  state.SetField(-1, "OpArgN", Lua51::OpArgN);
  state.SetField(-1, "OpArgU", Lua51::OpArgU);
  state.SetField(-1, "OpArgR", Lua51::OpArgR);
  state.SetField(-1, "OpArgK", Lua51::OpArgK);
  lua_setfield(state, -2, "OpArgMask");
  lua_setfield(state, -2, "ops");

  luaL_Reg regTable [] = {
    { "hasData",   _hasData },
    { "deleteData",   _deleteData },
    { "loadFromInstructionStream",   _loadFromInstructionStream },
    { "saveInstructionStream",   _saveInstructionStream },
    { "getStorageListing",  _getStorageListing },
    { "beginSubscriptionUpdate",  _beginSubscriptionUpdate },
    { "setTimeDilation",  _setTimeDilation },
    { "registerThread",  _registerThread },
    { "registerWrappedThread",  _registerWrappedThread },
    { "getDisassembly",  _getDisassembly },
    { "getAssetListing", _getAssetListing },
    { NULL, NULL }
  };
  luaL_register ( state, 0, regTable );
}

