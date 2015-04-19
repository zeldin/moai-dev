#if !MOAI_WITH_LUAJIT

#ifndef LUA5PORTABLEUNDUMP_H
#define LUA5PORTABLEUNDUMP_H

struct Proto;
struct Mbuffer;
typedef struct Zio ZIO;
union TString;

class Lua5PortableUndump
{
private:
  class ZIOReader : public ZLStream
  {
  private:
    ZIO* Z;
    size_t cursor;
    Lua5PortableUndump &undump;
  public:
    ZIOReader(ZIO *Z, Lua5PortableUndump &undump) : Z(Z), cursor(0), undump(undump) { }
    virtual u32 GetCaps() { return CAN_READ; }
    virtual size_t GetCursor() { return cursor; }
    virtual size_t ReadBytes(void* buffer, size_t size);
  };

  Lua5PortableUndump(lua_State* L, ZIO* Z, Mbuffer *buff, const char* name)
    : L(L), b(buff), name(name), zreader(Z, *this), reader(zreader) { }

  void error(const char* why);

  s8 LoadChar() { return reader.ReadChar(); }
  u8 LoadByte() { return reader.ReadByte(); }
  s32 LoadInt() { return reader.ReadInt(); }
  u32 LoadSize() { return reader.ReadSize(); }
  u32 LoadInstruction() { return reader.ReadInstruction(); }
  double LoadNumber() { return reader.ReadNumber(); }
  TString* LoadString();

  void LoadCode(Proto* f);
  void LoadConstants(Proto* f);
  void LoadDebug(Proto* f);
  Proto* LoadFunction(TString* p);

  static Proto *delegate(lua_State* L, ZIO* Z, Mbuffer *buff, const char* name, const char *header);

  lua_State* L;
  Mbuffer* b;
  const char* name;  
  ZIOReader zreader;
  Lua51::DumpReader reader;

public:
  static void InstallDelegate();
};

#endif

#endif
