#ifndef DFHACK_H
#define DFHACK_H

#include "Lua51.h"

class DFHack :
	public MOAIGlobalClass < DFHack, MOAILuaObject > {
private:
	  static STLString BuildLocalDocumentFilename( MOAILuaState &state, int idx );
	  static int	_hasData ( lua_State* L );
	  static int	_loadFromInstructionStream ( lua_State* L );
	  static int	_saveInstructionStream ( lua_State* L );
	  static int	_getStorageListing ( lua_State* L );
	  static int	_beginSubscriptionUpdate ( lua_State* L );
public:
	  DECL_LUA_SINGLETON ( DFHack )

	  //----------------------------------------------------------------//

	  DFHack	();
	  ~DFHack	();
	  void	RegisterLuaClass	( MOAILuaState& state );
};
#endif
