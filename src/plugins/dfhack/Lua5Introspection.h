#if !MOAI_WITH_LUAJIT

#ifndef LUA5INTROSPECTION_H
#define LUA5INTROSPECTION_H

union Closure;
struct Proto;

class Lua5Introspection
{
 private:
  static bool PushProto(MOAILuaState &state, Proto *proto);

 public:
  static bool PushDisassembly(MOAILuaState &state, const Closure *func);

};

#endif

#endif
