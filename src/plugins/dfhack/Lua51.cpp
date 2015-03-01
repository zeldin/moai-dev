#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>

#include <dfhack/Lua51.h>

//----------------------------------------------------------------//

const u8 Lua51::HEADER[HEADERSIZE] = {
  0x1b, 0x4c, 0x75, 0x61,  // LUA_SIGNATURE ("\033Lua")
  0x51, 0x00,              // Version 5.1

  0x01,    // Little endian      // These are properties of the
  0x04,    // 4-byte int         // dump, not of the machine running
  0x04,    // 4-byte size_t      // this code...
  0x04,    // 4-byte Instruction
  0x08,    // 8-byte Number
  0x00     // Floating point
};

//----------------------------------------------------------------//

bool Lua51::Function::create(MOAILuaState &state, int instrIdx, int constIdx, int maxStackSize)
{
  sizecode = lua_objlen(state, instrIdx);
  sizek = lua_objlen(state, constIdx);
  constants.SetRef(state, constIdx);
  this->maxStackSize = maxStackSize;
  if (code)
    delete[] code;
  code = new u32[sizecode];
  if (!code)
    return false;
  for (int i = 0; i < sizecode; i++) {
    state.GetField(instrIdx, i+1);
    int opcode = state.GetField<s16>(-1, "opcode", -1);
    int A = state.GetField<s16>(-1, "A", -1);
    int B = state.GetField<s16>(-1, "B", -1);
    int C = state.GetField<s16>(-1, "C", -1);
    s32 Bx = state.GetField<s32>(-1, "Bx", -1);
    state.Pop(1);
    if (opcode < 0 || opcode > 63 ||
	A < 0 || A > 0xff ||
	B < -1 || B > 0x1ff ||
	C < -1 || C > 0x1ff ||
	Bx < -1 || Bx > 0x3ffff ||
	(Bx >= 0 && (B >= 0 || C >= 0))) {
      MOAIPrint("invalid instruction\n");
      return false;
    }
    u32 instr = opcode | (A << 6);
    if (B >= 0) instr |= (B << 23);
    if (C >= 0) instr |= (C << 14);
    if (Bx >= 0) instr |= (Bx << 14);
    code[i] = instr;
  }
  return true;
}

//----------------------------------------------------------------//

void Lua51::Function::dump(DumpWriter &writer, MOAILuaState &state)
{
  writer.WriteString();
  writer.WriteInt(0);
  writer.WriteInt(0);
  writer.WriteByte(0);
  writer.WriteByte(0);
  writer.WriteByte(0);
  writer.WriteByte(maxStackSize);
  writer.WriteInt(sizecode);
  for (int i = 0; i<sizecode; i++)
    writer.WriteInstruction(code[i]);
  writer.WriteInt(sizek);
  for (int i = 0; i<sizek; i++) {
    pushConstant(state, i);
    switch (lua_type(state, -1)) {
    case LUA_TBOOLEAN:
      writer.WriteChar(TBOOLEAN);
      writer.WriteChar(lua_toboolean(state, -1));
      break;
    case LUA_TNUMBER:
      writer.WriteChar(TNUMBER);
      writer.WriteNumber(lua_tonumber(state, -1));
      break;
    case LUA_TSTRING:
      {
	size_t len;
	const char *s = lua_tolstring(state, -1, &len);
	writer.WriteChar(TSTRING);
	writer.WriteString(s, len+1);
      }
      break;

    default:
      MOAIPrint("bad constant\n");
      /* FALLTHRU */
    case LUA_TNIL:
      writer.WriteChar(TNIL);
      break;
    }
    state.Pop(1);
  }
  writer.WriteInt(0);
  writer.WriteInt(0);
  writer.WriteInt(0);
  writer.WriteInt(0);
}

//----------------------------------------------------------------//

bool Lua51::Function::undump(DumpReader& reader, MOAILuaState &state)
{
  STLString source = reader.ReadString();
  s32 lineDefined = reader.ReadInt();
  s32 lastLineDefined = reader.ReadInt();
  u8 nups = reader.ReadByte();
  u8 numparams = reader.ReadByte();
  u8 isVararg = reader.ReadByte();
  maxStackSize = reader.ReadByte();
  sizecode = reader.ReadInt();
  if (code)
    delete[] code;
  code = new u32[sizecode];
  if (!code)
    return false;
  for (int i = 0; i<sizecode; i++)
    code[i] = reader.ReadInstruction();
  sizek = reader.ReadInt();
  lua_newtable (state);
  for (int i = 0; i<sizek; i++) {
    lua_pushinteger(state, i+1);
    switch (reader.ReadChar()) {
    case TNIL:
      lua_pushnil(state);
      break;
    case TBOOLEAN:
      {
	bool v = reader.ReadChar() != 0;
	lua_pushboolean(state, v);
      }
      break;
    case TNUMBER:
      {
	lua_Number v = reader.ReadNumber();
	lua_pushnumber(state, v);
      }
      break;
    case TSTRING:
      {
	STLString v = reader.ReadString();
	lua_pushstring(state, v.str());
      }
      break;
    default:
      MOAIPrint("bad constant\n");
      state.Pop(2);
      return false;
    }
    lua_settable(state, -3);
  }
  constants.SetRef(state, -1);
  state.Pop(1);
  return true;
}

//----------------------------------------------------------------//

int Lua51::Function::interpret(MOAILuaState &state)
{
  int base = lua_gettop(state)+1;
  if (!lua_checkstack(state, maxStackSize)) {
    MOAIPrint("Out of stack\n");
    return 0;
  }
  lua_settop(state, base+maxStackSize-1);
  for (int i=0; i<sizecode; i++) {
    u32 instruction = code[i];
    int opcode = instruction & 0x3f;
    int a = (instruction >> 6) & 0xff;
    int b = (instruction >> 23) & 0x1ff;
    int c = (instruction >> 14) & 0x1ff;
    switch(opcode) {
    case OP_LOADK:
      {
	int bx = (b<<9)|c;
	pushConstant(state, bx);
	lua_replace(state, base+a);
	break;
      }
    case OP_SETTABLE:
      if (b<256)
	lua_pushvalue(state, base+b);
      else
	pushConstant(state, b-256);
      if (c<256)
	lua_pushvalue(state, base+c);
      else
	pushConstant(state, c-256);
      lua_settable(state, base+a);
      break;
    case OP_NEWTABLE:
      lua_createtable(state, b, c);
      lua_replace(state, base+a);
      break;
    case OP_RETURN:
      if (b == 0)
	return lua_gettop(state)-base+1-a;
      else {
	lua_settop(state, base+a+b-2);
	return b-1;
      }
    default:
      MOAIPrint("Unknown opcode %d\n", opcode);
      break;
    }
  }
  lua_settop(state, base-1);
  return 0;
}

//----------------------------------------------------------------//

void Lua51::Function::pushConstant(MOAILuaState &state, int idx)
{
  if (constants && idx < sizek) {
    constants.PushRef(state);
    state.GetField(-1, idx+1);
    lua_remove(state, -2);
  } else {
    lua_pushnil(state);
  }
}

//----------------------------------------------------------------//
