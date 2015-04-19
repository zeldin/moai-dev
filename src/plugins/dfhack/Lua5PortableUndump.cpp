#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>
#include <moai-sim/headers.h>

#if !MOAI_WITH_LUAJIT

#include <dfhack/Lua51.h>
#include <dfhack/Lua5PortableUndump.h>

extern "C" {
#include <lapi.h>
#include <lundump.h>
#include <ldo.h>
#include <lstring.h>
#include <lfunc.h>
#include <ldebug.h>
}

size_t Lua5PortableUndump::ZIOReader::ReadBytes(void* buffer, size_t size)
{
  size_t missing = luaZ_read(Z, buffer, size);
  size -= missing;
  cursor += size;
  if (missing) undump.error("unexpected end");
  return size;
}

void Lua5PortableUndump::error(const char* why)
{
  luaO_pushfstring(L,"%s: %s in precompiled chunk",name,why);
  luaD_throw(L,LUA_ERRSYNTAX);
}

TString* Lua5PortableUndump::LoadString()
{
  u32 size = LoadSize();
  if (size==0)
    return NULL;
  else {
    char* s=luaZ_openspace(L,b,size);
    zreader.ReadBytes(s,size);
    return luaS_newlstr(L,s,size-1); /* remove trailing '\0' */
 }
}

void Lua5PortableUndump::LoadCode(Proto* f)
{
 int n=LoadInt();
 f->code=luaM_newvector(L,n,Instruction);
 f->sizecode=n;
 for (int i=0; i<n; i++)
   f->code[i] = LoadInstruction();
}

void Lua5PortableUndump::LoadConstants(Proto* f)
{
 int i,n;
 n=LoadInt();
 f->k=luaM_newvector(L,n,TValue);
 f->sizek=n;
 for (i=0; i<n; i++) setnilvalue(&f->k[i]);
 for (i=0; i<n; i++)
 {
  TValue* o=&f->k[i];
  int t=LoadChar();
  switch (t)
  {
  case Lua51::TNIL:
    setnilvalue(o);
    break;
  case Lua51::TBOOLEAN:
    setbvalue(o,LoadChar());
    break;
  case Lua51::TNUMBER:
    setnvalue(o,LoadNumber());
    break;
  case Lua51::TSTRING:
    setsvalue2n(L,o,LoadString());
    break;
  default:
    error("bad constant");
    break;
  }
 }
 n=LoadInt();
 f->p=luaM_newvector(L,n,Proto*);
 f->sizep=n;
 for (i=0; i<n; i++) f->p[i]=NULL;
 for (i=0; i<n; i++) f->p[i]=LoadFunction(f->source);
}

void Lua5PortableUndump::LoadDebug(Proto* f)
{
 int i,n;
 n=LoadInt();
 f->lineinfo=luaM_newvector(L,n,int);
 f->sizelineinfo=n;
 for (i=0; i<n; i++)
   f->lineinfo[i] = LoadInt();
 n=LoadInt();
 f->locvars=luaM_newvector(L,n,LocVar);
 f->sizelocvars=n;
 for (i=0; i<n; i++) f->locvars[i].varname=NULL;
 for (i=0; i<n; i++)
 {
  f->locvars[i].varname=LoadString();
  f->locvars[i].startpc=LoadInt();
  f->locvars[i].endpc=LoadInt();
 }
 n=LoadInt();
 f->upvalues=luaM_newvector(L,n,TString*);
 f->sizeupvalues=n;
 for (i=0; i<n; i++) f->upvalues[i]=NULL;
 for (i=0; i<n; i++) f->upvalues[i]=LoadString();
}

Proto* Lua5PortableUndump::LoadFunction(TString* p)
{
  Proto* f=luaF_newproto(L);
  setptvalue2s(L,L->top,f); incr_top(L);
  f->source=LoadString(); if (f->source==NULL) f->source=p;
  f->linedefined=LoadInt();
  f->lastlinedefined=LoadInt();
  f->nups=LoadByte();
  f->numparams=LoadByte();
  f->is_vararg=LoadByte();
  f->maxstacksize=LoadByte();
  LoadCode(f);
  LoadConstants(f);
  LoadDebug(f);
  if (!luaG_checkcode(f)) error("bad code");
  L->top--;
  return f;
}

Proto *Lua5PortableUndump::delegate(lua_State* L, ZIO* Z, Mbuffer *buff, const char* name, const char *header)
{
  if (!memcmp(header, Lua51::HEADER, sizeof(Lua51::HEADER))) {
    Lua5PortableUndump undump(L, Z, buff, name);
    return undump.LoadFunction(luaS_newliteral(L,"=?"));
  } else
    return NULL;
}

void Lua5PortableUndump::InstallDelegate()
{
  luaU_set_undump_delegate(delegate);
}

#endif
