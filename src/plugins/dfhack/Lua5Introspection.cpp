#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>
#include <moai-sim/headers.h>

#if !MOAI_WITH_LUAJIT

#include <dfhack/Lua51.h>
#include <dfhack/Lua5Introspection.h>

extern "C" {
#include <lapi.h>
}

bool Lua5Introspection::PushProto(MOAILuaState &state, Proto *proto)
{
  lua_newtable(state);
  lua_newtable(state);
  for (int i = 0; i < proto->sizek; i++) {
    lua_pushinteger(state, i+1);
    luaA_pushobject(state, &proto->k[i]);
    lua_settable(state, -3);
  }
  lua_setfield(state, -2, "constants");
  lua_newtable(state);
  for (int i = 0; i < proto->sizecode; i++) {
    lua_pushinteger(state, i+1);
    Lua51::PushInstruction(state, proto->code[i]);
    lua_settable(state, -3);
  }
  lua_setfield(state, -2, "code");
  lua_newtable(state);
  for (int i = 0; i < proto->sizep; i++) {
    lua_pushinteger(state, i+1);
    if (proto->p[i]) {
      if (!PushProto(state, proto->p[i]))
	return false;
    } else
      lua_pushnil(state);
    lua_settable(state, -3);
  }
  lua_setfield(state, -2, "protos");
  if (proto->source)
    lua_pushstring(state, getstr(proto->source));
  else
    lua_pushnil(state);
  lua_setfield(state, -2, "source");
  lua_pushinteger(state, proto->linedefined);
  lua_setfield(state, -2, "linedefined");
  lua_pushinteger(state, proto->lastlinedefined);
  lua_setfield(state, -2, "lastlinedefined");
  lua_pushinteger(state, proto->nups);
  lua_setfield(state, -2, "nups");
  lua_pushinteger(state, proto->numparams);
  lua_setfield(state, -2, "numparams");
  lua_pushboolean(state, proto->is_vararg);
  lua_setfield(state, -2, "is_vararg");
  lua_pushinteger(state, proto->maxstacksize);
  lua_setfield(state, -2, "maxstacksize");
  lua_newtable(state);
  lua_newtable(state);
  for (int i = 0; i < proto->sizelineinfo; i++) {
    lua_pushinteger(state, i+1);
    lua_pushinteger(state, proto->lineinfo[i]);
    lua_settable(state, -3);
  }
  lua_setfield(state, -2, "lineinfo");
  lua_newtable(state);
  for (int i = 0; i < proto->sizelocvars; i++) {
    lua_pushinteger(state, i+1);
    lua_newtable(state);
    if (proto->locvars[i].varname)
      lua_pushstring(state, getstr(proto->locvars[i].varname));
    else
      lua_pushnil(state);
    lua_setfield(state, -2, "varname");
    lua_pushinteger(state, proto->locvars[i].startpc);
    lua_setfield(state, -2, "startpc");
    lua_pushinteger(state, proto->locvars[i].endpc);
    lua_setfield(state, -2, "endpc");
    lua_settable(state, -3);
  }
  lua_setfield(state, -2, "locvars");
  lua_newtable(state);
  for (int i = 0; i < proto->sizeupvalues; i++) {
    lua_pushinteger(state, i+1);
    if (proto->upvalues[i])
      lua_pushstring(state, getstr(proto->upvalues[i]));
    else
      lua_pushnil(state);
    lua_settable(state, -3);
  }
  lua_setfield(state, -2, "upvalues");
  lua_setfield(state, -2, "debug");
  return true;
}

bool Lua5Introspection::PushDisassembly(MOAILuaState &state, const Closure *func)
{
  if (func->l.isC) {
    MOAIPrint("Trying to disassemble a C function\n");
    return false;
  }
  lua_newtable(state);
  if (!PushProto(state, func->l.p))
    return false;
  lua_setfield(state, -2, "proto");
  return true;
}

#endif
