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

bool Lua51::Function::LocVar::create(MOAILuaState &state, int idx)
{
  if (state.GetFieldWithType(idx, "varname", LUA_TSTRING)) {
    size_t len = 0;
    const char *str = lua_tolstring (state, -1, &len);
    len++;
    varname = STLString(str, len);
    state.Pop(1);
  }
  startpc = state.GetField<s32>(idx, "startpc", 0);
  endpc = state.GetField<s32>(idx, "endpc", 0);
  return true;
}

//----------------------------------------------------------------//

void Lua51::Function::LocVar::dump(DumpWriter &writer)
{
  writer.WriteString(varname.c_str(), varname.length());
  writer.WriteInt(startpc);
  writer.WriteInt(endpc);
}

//----------------------------------------------------------------//

bool Lua51::Function::create(MOAILuaState &state, int instrIdx, int constIdx, int maxStackSize)
{
  if (source)
    delete[] source;
  source = NULL;
  sizesource = 0;
  if (protos)
    delete[] protos;
  protos = NULL;
  numprotos = 0;
  if (lineinfo)
    delete[] lineinfo;
  lineinfo = NULL;
  sizelineinfo = 0;
  if (locvars)
    delete[] locvars;
  locvars = NULL;
  sizelocvars = 0;
  if (upvalues)
    delete[] upvalues;
  upvalues = NULL;
  sizeupvalues = 0;
  sizecode = lua_objlen(state, instrIdx);
  sizek = lua_objlen(state, constIdx);
  constants.SetRef(state, constIdx);
  lineDefined = 0;
  lastLineDefined = 0;
  nUps = 0;
  numParams = 0;
  isVararg = 0;
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

bool Lua51::Function::create(MOAILuaState &state, int idx)
{
  if (source)
    delete[] source;
  if (state.GetFieldWithType(idx, "source", LUA_TSTRING)) {
    size_t len = 0;
    const char *str = lua_tolstring (state, -1, &len);
    len++;
    source = new char[len];
    if (!source)
      return false;
    sizesource = len;
    memcpy(source, str, len);
    state.Pop(1);
  } else {
    source = NULL;
    sizesource = 0;
  }
  lineDefined = state.GetField<s32>(idx, "linedefined", 0);
  lastLineDefined = state.GetField<s32>(idx, "lastlinedefined", 0);
  nUps = state.GetField<u8>(idx, "nups", 0);
  numParams = state.GetField<u8>(idx, "numparams", 0);
  if (state.GetField<bool>(idx, "is_vararg", false))
    isVararg = 2;
  else
    isVararg = state.GetField<u8>(idx, "is_vararg", 0);
  maxStackSize = state.GetField<u8>(idx, "maxstacksize", 0);
  if (code)
    delete[] code;
  if (state.GetFieldWithType(idx, "code", LUA_TTABLE)) {
    sizecode = lua_objlen(state, -1);
    code = new u32[sizecode];
    if (!code)
      return false;
    for (int i = 0; i < sizecode; i++) {
      state.GetField(-1, i+1);
      code[i] = state.GetField<u32>(-1, "instruction", 0);
      state.Pop(1);
    }
    state.Pop(1);
  } else {
    sizecode = 0;
    code = NULL;
  }
  if (state.GetFieldWithType(idx, "constants", LUA_TTABLE)) {
    sizek = lua_objlen(state, -1);
    constants.SetRef(state, -1);
    state.Pop(1);
  } else {
    sizek = 0;
    constants.Clear();
  }
  if (protos)
    delete[] protos;
  if (state.GetFieldWithType(idx, "protos", LUA_TTABLE)) {
    numprotos = lua_objlen(state, -1);
    if (numprotos) {
      protos = new Function[numprotos];
      if (!protos)
	return false;
      for (int i = 0; i < numprotos; i++) {
	state.GetField(-1, i+1);
	protos[i].create(state, -1);
	state.Pop(1);
      }
    } else
      protos = NULL;
    state.Pop(1);
  } else {
    numprotos = 0;
    protos = NULL;
  }
  if (lineinfo)
    delete[] lineinfo;
  sizelineinfo = 0;
  lineinfo = NULL;
  if (locvars)
    delete[] locvars;
  sizelocvars = 0;
  locvars = NULL;
  if (upvalues)
    delete[] upvalues;
  sizeupvalues = 0;
  upvalues = NULL;
  if (state.GetFieldWithType(idx, "debug", LUA_TTABLE)) {
    if (state.GetFieldWithType(-1, "lineinfo", LUA_TTABLE)) {
      sizelineinfo = lua_objlen(state, -1);
      if (sizelineinfo) {
	lineinfo = new s32[sizelineinfo];
	if (!lineinfo)
	  return false;
	for (int i = 0; i < sizelineinfo; i++) {
	  lineinfo[i] = state.GetField<s32>(-1, i+1, 0);
	}
      }
      state.Pop(1);
    }
    if (state.GetFieldWithType(-1, "locvars", LUA_TTABLE)) {
      sizelocvars = lua_objlen(state, -1);
      if (sizelocvars) {
	locvars = new LocVar[sizelocvars];
	if (!locvars)
	  return false;
	for (int i = 0; i < sizelocvars; i++) {
	  state.GetField(-1, i+1);
	  if (!locvars[i].create(state, -1))
	    return false;
	  state.Pop(1);
	}
      }
      state.Pop(1);
    }
    if (state.GetFieldWithType(-1, "upvalues", LUA_TTABLE)) {
      sizeupvalues = lua_objlen(state, -1);
      if (sizeupvalues) {
	upvalues = new STLString[sizeupvalues];
	if (!upvalues)
	  return false;
	for (int i = 0; i < sizeupvalues; i++) {
	  if (state.GetFieldWithType(-1, i+1, LUA_TSTRING)) {
	    size_t len = 0;
	    const char *str = lua_tolstring (state, -1, &len);
	    len++;
	    upvalues[i] = STLString(str, len);
	    state.Pop(1);
	  }
	}
      }
      state.Pop(1);
    }
    state.Pop(1);
  }
  return true;
}

//----------------------------------------------------------------//

void Lua51::Function::dump(DumpWriter &writer, MOAILuaState &state)
{
  writer.WriteString(source, sizesource);
  writer.WriteInt(lineDefined);
  writer.WriteInt(lastLineDefined);
  writer.WriteByte(nUps);
  writer.WriteByte(numParams);
  writer.WriteByte(isVararg);
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
  if (protos) {
    writer.WriteInt(numprotos);
    for (int i=0; i<numprotos; i++)
      protos[i].dump(writer, state);
  } else
    writer.WriteInt(0);
  writer.WriteInt(sizelineinfo);
  for (int i=0; i<sizelineinfo; i++)
    writer.WriteInt(lineinfo[i]);
  writer.WriteInt(sizelocvars);
  for (int i=0; i<sizelocvars; i++)
    locvars[i].dump(writer);
  writer.WriteInt(sizeupvalues);
  for (int i=0; i<sizeupvalues; i++)
    writer.WriteString(upvalues[i].c_str(), upvalues[i].length());
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

const char * const Lua51::OpcodeName[NUM_OPCODES] = {
  "MOVE",
  "LOADK",
  "LOADBOOL",
  "LOADNIL",
  "GETUPVAL",
  "GETGLOBAL",
  "GETTABLE",
  "SETGLOBAL",
  "SETUPVAL",
  "SETTABLE",
  "NEWTABLE",
  "SELF",
  "ADD",
  "SUB",
  "MUL",
  "DIV",
  "MOD",
  "POW",
  "UNM",
  "NOT",
  "LEN",
  "CONCAT",
  "JMP",
  "EQ",
  "LT",
  "LE",
  "TEST",
  "TESTSET",
  "CALL",
  "TAILCALL",
  "RETURN",
  "FORLOOP",
  "FORPREP",
  "TFORLOOP",
  "SETLIST",
  "CLOSE",
  "CLOSURE",
  "VARARG"
};

//----------------------------------------------------------------//

#define opmode(t,a,b,c,m) (((t)<<7) | ((a)<<6) | ((b)<<4) | ((c)<<2) | (m))

const u8 Lua51::OpModes[NUM_OPCODES] = {
/*       T  A    B       C     mode		   opcode	*/
  opmode(0, 1, OpArgR, OpArgN, iABC) 		/* OP_MOVE */
 ,opmode(0, 1, OpArgK, OpArgN, iABx)		/* OP_LOADK */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_LOADBOOL */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_LOADNIL */
 ,opmode(0, 1, OpArgU, OpArgN, iABC)		/* OP_GETUPVAL */
 ,opmode(0, 1, OpArgK, OpArgN, iABx)		/* OP_GETGLOBAL */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_GETTABLE */
 ,opmode(0, 0, OpArgK, OpArgN, iABx)		/* OP_SETGLOBAL */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_SETUPVAL */
 ,opmode(0, 0, OpArgK, OpArgK, iABC)		/* OP_SETTABLE */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_NEWTABLE */
 ,opmode(0, 1, OpArgR, OpArgK, iABC)		/* OP_SELF */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_ADD */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_SUB */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_MUL */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_DIV */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_MOD */
 ,opmode(0, 1, OpArgK, OpArgK, iABC)		/* OP_POW */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_UNM */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_NOT */
 ,opmode(0, 1, OpArgR, OpArgN, iABC)		/* OP_LEN */
 ,opmode(0, 1, OpArgR, OpArgR, iABC)		/* OP_CONCAT */
 ,opmode(0, 0, OpArgR, OpArgN, iAsBx)		/* OP_JMP */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_EQ */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_LT */
 ,opmode(1, 0, OpArgK, OpArgK, iABC)		/* OP_LE */
 ,opmode(1, 1, OpArgR, OpArgU, iABC)		/* OP_TEST */
 ,opmode(1, 1, OpArgR, OpArgU, iABC)		/* OP_TESTSET */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_CALL */
 ,opmode(0, 1, OpArgU, OpArgU, iABC)		/* OP_TAILCALL */
 ,opmode(0, 0, OpArgU, OpArgN, iABC)		/* OP_RETURN */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_FORLOOP */
 ,opmode(0, 1, OpArgR, OpArgN, iAsBx)		/* OP_FORPREP */
 ,opmode(1, 0, OpArgN, OpArgU, iABC)		/* OP_TFORLOOP */
 ,opmode(0, 0, OpArgU, OpArgU, iABC)		/* OP_SETLIST */
 ,opmode(0, 0, OpArgN, OpArgN, iABC)		/* OP_CLOSE */
 ,opmode(0, 1, OpArgU, OpArgN, iABx)		/* OP_CLOSURE */
 ,opmode(0, 1, OpArgU, OpArgN, iABC)		/* OP_VARARG */
};

//----------------------------------------------------------------//

int Lua51::_GET_OPCODE_NAME ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
  int opcode = instruction & 0x3f;
  lua_pushstring(state, (opcode < NUM_OPCODES? OpcodeName[opcode] : "???"));
  return 1;
}

//----------------------------------------------------------------//

int Lua51::_getOpMode ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
  int opcode = instruction & 0x3f;
  lua_pushinteger(state, (opcode < NUM_OPCODES? (OpModes[opcode]&3) : 0));
  return 1;
}

//----------------------------------------------------------------//

int Lua51::_getCMode ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
  int opcode = instruction & 0x3f;
  lua_pushinteger(state, (opcode < NUM_OPCODES? ((OpModes[opcode]>>2)&3) : 0));
  return 1;
}

//----------------------------------------------------------------//

int Lua51::_getBMode ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
  int opcode = instruction & 0x3f;
  lua_pushinteger(state, (opcode < NUM_OPCODES? ((OpModes[opcode]>>4)&3) : 0));
  return 1;
}

//----------------------------------------------------------------//

int Lua51::_testAMode ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
  int opcode = instruction & 0x3f;
  lua_pushboolean(state, (opcode < NUM_OPCODES? ((OpModes[opcode]>>6)&1) : 0));
  return 1;
}

//----------------------------------------------------------------//

int Lua51::_GETARG_A ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));

  int a = (instruction >> 6) & 0xff;
  lua_pushinteger(state, a);

  return 1;
}

//----------------------------------------------------------------//

int Lua51::_GETARG_B ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));

  int b = (instruction >> 23) & 0x1ff;
  lua_pushinteger(state, b);

  return 1;
}

//----------------------------------------------------------------//

int Lua51::_GETARG_B_FB2INT ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));

  int b = (instruction >> 23) & 0x1ff;
  int e = (b >> 3) & 31;
  if(e) b = ((b & 7)+8) << (e - 1);
  lua_pushinteger(state, b);

  return 1;
}

//----------------------------------------------------------------//

int Lua51::_GETARG_C ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));

  int c = (instruction >> 14) & 0x1ff;
  lua_pushinteger(state, c);

  return 1;
}

//----------------------------------------------------------------//

int Lua51::_GETARG_C_FB2INT ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));

  int c = (instruction >> 14) & 0x1ff;
  int e = (c >> 3) & 31;
  if(e) c = ((c & 7)+8) << (e - 1);
  lua_pushinteger(state, c);

  return 1;
}

//----------------------------------------------------------------//

int Lua51::_GETARG_Bx ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));

  int bx = (instruction >> 14) & 0x3ffff;
  lua_pushinteger(state, bx);

  return 1;
}

//----------------------------------------------------------------//

int Lua51::_GETARG_sBx ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));

  int bx = (instruction >> 14) & 0x3ffff;
  bx -= 0x1ffff;
  lua_pushinteger(state, bx);

  return 1;
}

//----------------------------------------------------------------//

int Lua51::Instruction__index ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "US" )) return 0;
  size_t ilen;
  const char *index = lua_tolstring(state, 2, &ilen);
  switch(ilen) {
  case 8:
    if (index[0] == 'G')
      switch(index[7]) {
      case 'A':
	if (!strcmp(index, "GETARG_A")) {
	  lua_pushcfunction(state, _GETARG_A);
	  return 1;
	}
	break;
      case 'B':
	if (!strcmp(index, "GETARG_B")) {
	  lua_pushcfunction(state, _GETARG_B);
	  return 1;
	}
	break;
      case 'C':
	if (!strcmp(index, "GETARG_C")) {
	  lua_pushcfunction(state, _GETARG_C);
	  return 1;
	}
	break;
      }
    else if (index[0] == 'g')
      switch(index[3]) {
      case 'B':
	if (!strcmp(index, "getBMode")) {
	  lua_pushcfunction(state, _getBMode);
	  return 1;
	}
	break;
      case 'C':
	if (!strcmp(index, "getCMode")) {
	  lua_pushcfunction(state, _getCMode);
	  return 1;
	}
	break;
      }
    break;
  case 9:
    switch(index[0]) {
    case 'G':
      if (!strcmp(index, "GETARG_Bx")) {
	lua_pushcfunction(state, _GETARG_Bx);
	return 1;
      }
      break;
    case 'g':
      if (!strcmp(index, "getOpMode")) {
	lua_pushcfunction(state, _getOpMode);
	return 1;
      }
      break;
    case 't':
      if (!strcmp(index, "testAMode")) {
	lua_pushcfunction(state, _testAMode);
	return 1;
      }
      break;
    }
    break;
  case 10:
    if (!strcmp(index, "GETARG_sBx")) {
      lua_pushcfunction(state, _GETARG_sBx);
      return 1;
    }
    break;
  case 11:
    if (!strcmp(index, "instruction")) {
      u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
      lua_pushinteger(state, instruction);
      return 1;
    }
    break;
  case 15:
    switch(index[7]) {
    case 'B':
      if (!strcmp(index, "GETARG_B_FB2INT")) {
	lua_pushcfunction(state, _GETARG_B_FB2INT);
	return 1;
      }
      break;
    case 'C':
      if (!strcmp(index, "GETARG_C_FB2INT")) {
	lua_pushcfunction(state, _GETARG_C_FB2INT);
	return 1;
      }
      break;
    case 'O':
      if (!strcmp(index, "GET_OPCODE_NAME")) {
	lua_pushcfunction(state, _GET_OPCODE_NAME);
	return 1;
      }
      break;
    }
    break;
  }

  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
  fprintf(stderr, "Index %s %u\n", index, (unsigned)instruction);
  return 0;
}

//----------------------------------------------------------------//

int Lua51::Instruction__tostring ( lua_State* L )
{
  MOAILuaState state ( L );
  if ( !state.CheckParams ( 1, "U" )) return 0;
  u32 instruction = *static_cast<u32 *>(lua_touserdata(state, 1));
  int opcode = instruction & 0x3f;
  char buf[16];
  snprintf(buf, sizeof(buf), "0x%08x", (unsigned)instruction);
  lua_pushstring(state, buf);
  return 1;
}

//----------------------------------------------------------------//

void Lua51::PushInstruction(MOAILuaState &state, u32 instruction)
{
  *static_cast<u32 *>(lua_newuserdata(state, sizeof(u32))) = instruction;
  lua_newtable(state);
  lua_pushcfunction(state, Instruction__index);
  lua_setfield(state, -2, "__index");
  lua_pushcfunction(state, Instruction__tostring);
  lua_setfield(state, -2, "__tostring");
  lua_setmetatable(state, -2);
}

//----------------------------------------------------------------//
