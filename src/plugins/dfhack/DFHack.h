#ifndef DFHACK_H
#define DFHACK_H

#include "Lua51.h"

class DFHack :
	public MOAIGlobalClass < DFHack, MOAILuaObject > {
private:
	  static int	_loadFromInstructionStream ( lua_State* L );
	  static int	_saveInstructionStream ( lua_State* L );
public:
	  DECL_LUA_SINGLETON ( DFHack )

	  //----------------------------------------------------------------//

	  DFHack	();
	  ~DFHack	();
	  void	RegisterLuaClass	( MOAILuaState& state );
};
#endif
