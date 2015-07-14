#ifndef LUA51_H
#define LUA51_H

#include <endian.h>
#include <byteswap.h>

class Lua51
{
  /* Magic values from Lua 5.1.3 lopcodes.h.  Do _not_ change the order... */
 public:
  typedef enum {
    OP_MOVE,
    OP_LOADK,
    OP_LOADBOOL,
    OP_LOADNIL,
    OP_GETUPVAL,
    OP_GETGLOBAL,
    OP_GETTABLE,
    OP_SETGLOBAL,
    OP_SETUPVAL,
    OP_SETTABLE,
    OP_NEWTABLE,
    OP_SELF,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_POW,
    OP_UNM,
    OP_NOT,
    OP_LEN,
    OP_CONCAT,
    OP_JMP,
    OP_EQ,
    OP_LT,
    OP_LE,
    OP_TEST,
    OP_TESTSET,
    OP_CALL,
    OP_TAILCALL,
    OP_RETURN,
    OP_FORLOOP,
    OP_FORPREP,
    OP_TFORLOOP,
    OP_SETLIST,
    OP_CLOSE,
    OP_CLOSURE,
    OP_VARARG,
    NUM_OPCODES
  } Opcode;

  typedef enum {
    iABC,
    iABx,
    iAsBx
  } OpMode;

  typedef enum {
    OpArgN,
    OpArgU,
    OpArgR,
    OpArgK
  } OpArgMask;

  static const int TNIL = 0;
  static const int TBOOLEAN = 1;
  static const int TNUMBER = 3;
  static const int TSTRING = 4;

  static const int HEADERSIZE = 12;
  static const u8 HEADER[HEADERSIZE];

  class DumpWriter
  {
  private:
#if __BYTE_ORDER == __LITTLE_ENDIAN
    static u32 ToLE32(u32 v) { return v; }
    static u64 ToLE64(u64 v) { return v; }
#else
    static u32 ToLE32(u32 v) { return __bswap_32(v); }
    static u64 ToLE64(u64 v) { return __bswap_64(v); }
#endif

    void WriteU32(u32 v) { stream.Write<u32>(ToLE32(v)); }
    void WriteU64(u64 v) { stream.Write<u64>(ToLE64(v)); }

  public:
    DumpWriter(ZLStream &stream) : stream(stream) {}

    void WriteHeader()
    {
      stream.WriteBytes(HEADER, sizeof(HEADER));
    }

    void WriteChar(s8 v) { stream.Write<s8>(v); }
    void WriteByte(u8 v) { stream.Write<u8>(v); }
    void WriteInt(s32 v) { WriteU32((u32)v); }
    void WriteSize(u32 v) { WriteU32(v); }
    void WriteInstruction(u32 v) { WriteU32(v); }
    void WriteNumber(double v) {
      WriteU64(*reinterpret_cast<u64*>(&v));
    }
    void WriteString(const char *s = NULL, size_t l = 0) {
      if (s == NULL) 
	WriteSize(0);
      else {
	if (l == 0) l = strlen(s)+1;
	WriteSize(l);
	stream.WriteBytes(s, l);
      }
    }

  private:
    ZLStream &stream;
  };

  class DumpReader
  {
  private:
#if __BYTE_ORDER == __LITTLE_ENDIAN
    static u32 FromLE32(u32 v) { return v; }
    static u64 FromLE64(u64 v) { return v; }
#else
    static u32 FromLE32(u32 v) { return __bswap_32(v); }
    static u64 FromLE64(u64 v) { return __bswap_64(v); }
#endif

    u32 ReadU32() { return FromLE32(stream.Read<u32>(0)); }
    u64 ReadU64() { return FromLE64(stream.Read<u64>(0)); }

  public:
    DumpReader(ZLStream &stream) : stream(stream) {}

    bool CheckHeader()
    {
      u8 header[sizeof(HEADER)];

      if (stream.ReadBytes(header, sizeof(header)) != sizeof(header))
	return false;

      if (memcmp(header, HEADER, sizeof(header))) {
	return false;
      }

      return true;
    }

    s8 ReadChar() { return stream.Read<s8>(0); }
    u8 ReadByte() { return stream.Read<u8>(0); }
    s32 ReadInt() { return (s32)ReadU32(); }
    u32 ReadSize() { return ReadU32(); }
    u32 ReadInstruction() { return ReadU32(); }
    double ReadNumber() {
      u64 v = ReadU64();
      return *reinterpret_cast<double*>(&v);
    }

    STLString ReadString() {
      u32 len = ReadSize();
      if (len > 0) {
	return stream.ReadString(len).clip_to_front(len-1);
      } else
	return "";
    }

  private:
    ZLStream &stream;
  };

  class Function {
  private:
    class LocVar {
    public:
      LocVar() : startpc(0), endpc(0) {}
      ~LocVar() {}
      bool create(MOAILuaState &state, int idx);
      void dump(DumpWriter &writer);
    private:
      s32 startpc, endpc;
      STLString varname;
    };

  public:
    Function() : sizecode(0), sizelineinfo(0), sizelocvars(0), sizeupvalues(0),
      sizek(0), sizesource(0), numprotos(0), code(NULL), lineinfo(NULL),
      locvars(NULL), upvalues(NULL), source(NULL), protos(NULL), lineDefined(0),
      lastLineDefined(0), nUps(0), numParams(0), isVararg(0), maxStackSize(0) {}
    ~Function() {
      if(code) delete[] code;
      if(lineinfo) delete[] lineinfo;
      if(locvars) delete[] locvars;
      if(upvalues) delete[] upvalues;
      if(source) delete[] source;
      if(protos) delete[] protos;
    }
    bool create(MOAILuaState &state, int instrIdx, int constIdx, int maxStackSize);
    bool create(MOAILuaState &state, int idx);
    void dump(DumpWriter &writer, MOAILuaState &state, const Function *parent=NULL);
    bool undump(DumpReader &reader, MOAILuaState &state);
    int interpret(MOAILuaState &state);
  private:
    void pushConstant(MOAILuaState &state, int idx);
    s32 sizecode, sizelineinfo, sizelocvars, sizeupvalues;
    s32 sizek, sizesource, numprotos;
    u32 *code;
    s32 *lineinfo;
    LocVar *locvars;
    STLString *upvalues;
    char *source;
    MOAILuaStrongRef constants;
    Function *protos;
    s32 lineDefined, lastLineDefined;
    u8 nUps, numParams, isVararg, maxStackSize;
  };

  static void PushInstruction(MOAILuaState &state, u32 instruction);
 private:
  static const char * const OpcodeName[NUM_OPCODES];
  static const u8 OpModes[NUM_OPCODES];

  static int	Instruction__index ( lua_State* L );
  static int	Instruction__tostring ( lua_State* L );
  static int	_GET_OPCODE_NAME ( lua_State* L );
  static int	_getOpMode ( lua_State* L );
  static int	_getCMode ( lua_State* L );
  static int	_getBMode ( lua_State* L );
  static int	_testAMode ( lua_State* L );
  static int	_GETARG_A ( lua_State* L );
  static int	_GETARG_B ( lua_State* L );
  static int	_GETARG_B_FB2INT ( lua_State* L );
  static int	_GETARG_C ( lua_State* L );
  static int	_GETARG_C_FB2INT ( lua_State* L );
  static int	_GETARG_Bx ( lua_State* L );
  static int	_GETARG_sBx ( lua_State* L );
};

#endif
