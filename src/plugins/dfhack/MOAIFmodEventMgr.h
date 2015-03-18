#ifndef	MOAIFMODEVENTMGR_H
#define	MOAIFMODEVENTMGR_H

#include <moai-sim/headers.h>

class MOAIFmodMicrophone;

class MOAIFmodEventMgr :
	public MOAIGlobalClass < MOAIFmodEventMgr, MOAILuaObject > {
private:
	MOAIFmodMicrophone*                         mMic;

	//----------------------------------------------------------------//
	static int	_getMemoryStats		        ( lua_State* L );
	static int	_init				        ( lua_State* L );
    
    // project interface
    static int _loadProject                ( lua_State* L );

    // sound category interface
    static int _setSoundCategoryVolume     ( lua_State* L );
    static int _getSoundCategoryVolume     ( lua_State* L );
    static int _pauseSoundCategory         ( lua_State* L );

    // event interface
    static int _playEvent3D                ( lua_State* L );
    static int _playEvent2D                ( lua_State* L );    
    static int _playRawSound               ( lua_State* L );    
    static int _stopAllEvents              ( lua_State* L );

    // game mic interface
    static int _getMicrophone              ( lua_State* L );

    // reverb/global processing interface
    static int _clearDefaultReverb         ( lua_State* L );
    static int _setDefaultReverb           ( lua_State* L );

    // debug interface
    static int _debugGetAllDuckedCategories( lua_State* L );
    static int _debugGetAllEventInstances  ( lua_State* L );
    static int _debugGetMemoryInfo         ( lua_State* L );

    // other stuff
    static int _set3DRolloff               ( lua_State* L );
    static int _setGlobalPitch             ( lua_State* L );

public:

	DECL_LUA_SINGLETON ( MOAIFmodEventMgr )

	//----------------------------------------------------------------//
					                MOAIFmodEventMgr	();
	virtual			                ~MOAIFmodEventMgr	();
    void			                RegisterLuaClass	( MOAILuaState& state );
    void                            RegisterLuaFuncs	( MOAILuaState& state );
    MOAIFmodMicrophone*		        AffirmMic			();

};

#endif
