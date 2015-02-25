#ifndef DFHACK_H
#define DFHACK_H

class DFHack :
	public MOAIGlobalClass < DFHack, MOAILuaObject > {
private:
public:
	  DECL_LUA_SINGLETON ( DFHack )

	  //----------------------------------------------------------------//

	  DFHack	();
	  ~DFHack	();
	  void	RegisterLuaClass	( MOAILuaState& state );
};
#endif
