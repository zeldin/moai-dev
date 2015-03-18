#include <dfhack/MOAIFmodEventMgr.h>
#include <dfhack/MOAIFmodEventInstance.h>
#include <dfhack/MOAIFmodMicrophone.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_getMemoryStats ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_init ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_loadProject ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_setSoundCategoryVolume ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_getSoundCategoryVolume ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_pauseSoundCategory ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_playEvent3D ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  MOAILuaState state ( L );
  MOAIFmodEventInstance* pEventInstance = new MOAIFmodEventInstance();
  pEventInstance->PushLuaUserdata ( state );
  return 1;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_playEvent2D ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  MOAILuaState state ( L );
  MOAIFmodEventInstance* pEventInstance = new MOAIFmodEventInstance();
  pEventInstance->PushLuaUserdata ( state );
  return 1;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_playRawSound ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_stopAllEvents ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_getMicrophone ( lua_State* L ) {
    MOAILuaState state ( L );

    MOAIFmodMicrophone* pMic = MOAIFmodEventMgr::Get().AffirmMic();
    pMic->PushLuaUserdata ( state );

    return 1;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_clearDefaultReverb ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_setDefaultReverb ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_debugGetAllDuckedCategories ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_debugGetAllEventInstances ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_debugGetMemoryInfo ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_set3DRolloff ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//

int	MOAIFmodEventMgr::_setGlobalPitch ( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//================================================================//
// MOAIFmodEventMgr
//================================================================//

MOAIFmodEventMgr::MOAIFmodEventMgr () : mMic ( 0 ) {
    RTTI_SINGLE ( MOAIFmodEventMgr )
}

//----------------------------------------------------------------//
MOAIFmodEventMgr::~MOAIFmodEventMgr () {
}

//----------------------------------------------------------------//
MOAIFmodMicrophone* MOAIFmodEventMgr::AffirmMic () {

    if ( !this->mMic ) {
        this->mMic = new MOAIFmodMicrophone ();
        this->LuaRetain ( this->mMic );
    }
    return this->mMic;
}

//----------------------------------------------------------------//
void MOAIFmodEventMgr::RegisterLuaClass ( MOAILuaState& state ) {
    UNUSED ( state );
	
	luaL_Reg regTable [] = {
		{ "getMemoryStats",		                _getMemoryStats },
		{ "init",				                _init },

        { "loadProject",                        _loadProject },

        { "setSoundCategoryVolume",             _setSoundCategoryVolume },        
        { "getSoundCategoryVolume",             _getSoundCategoryVolume },        
        { "pauseSoundCategory",                 _pauseSoundCategory },        
        
        { "playEvent3D",                        _playEvent3D },        
        { "playEvent2D",                        _playEvent2D },     
        { "playRawSound",                       _playRawSound },
        { "stopAllEvents",                      _stopAllEvents },  
        
        { "getMicrophone",                      _getMicrophone },

	{ "clearDefaultReverb",                 _clearDefaultReverb },
	{ "setDefaultReverb",                   _setDefaultReverb },

	{ "debugGetAllDuckedCategories",        _debugGetAllDuckedCategories },
	{ "debugGetAllEventInstances",          _debugGetAllEventInstances },
	{ "debugGetMemoryInfo",                 _debugGetMemoryInfo },

	{ "set3DRolloff",                       _set3DRolloff },
	{ "setGlobalPitch",                     _setGlobalPitch },

		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIFmodEventMgr::RegisterLuaFuncs ( MOAILuaState& state ) {
	UNUSED ( state );
}

