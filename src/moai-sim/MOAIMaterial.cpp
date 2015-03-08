#include "pch.h"
#include <moai-sim/MOAIMaterial.h>
#include <moai-sim/MOAIGfxDevice.h>
#include <moai-sim/MOAIShader.h>

//----------------------------------------------------------------//

int MOAIMaterial::_setShader		( lua_State* L ) {

	MOAI_LUA_SETUP ( MOAIMaterial, "U" )

	MOAIShader* shader = state.GetLuaObject < MOAIShader >( 2, true );
	self->SetDependentMember < MOAIShader >( self->mShader, shader );

	return 0;
}

int MOAIMaterial::_setShaderValue		( lua_State* L ) {
  //fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setDynamicShaderValue	( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setTexture		( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setColor		( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setCullMode		( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setDepthTest		( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setDepthWrite		( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setBlendMode		( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIMaterial, "U" )

	if ( state.IsType ( 2, LUA_TNUMBER )) {
		if ( state.IsType ( 3, LUA_TNUMBER )) {

			u32 srcFactor = state.GetValue < u32 >( 2, 0 );
			u32 dstFactor = state.GetValue < u32 >( 3, 0 );
			self->mBlendMode.SetBlend ( srcFactor, dstFactor );
		}
		else {

			u32 blendMode = state.GetValue < u32 >( 2, MOAIBlendMode::BLEND_NORMAL );
			self->mBlendMode.SetBlend ( blendMode );
		}
	}
	else {
		self->mBlendMode.SetBlend ( MOAIBlendMode::BLEND_NORMAL );
	}

	self->ScheduleUpdate ();

	return 0;
}

int MOAIMaterial::_clearShaderValues	( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_clearDynamicShaderValues( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_resetPermutationState	( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setPermutationFlag	( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

int MOAIMaterial::_setPermutationSwitch	( lua_State* L ) {
  fprintf(stderr, "STUB: %s\n", __PRETTY_FUNCTION__);
  return 0;
}

//----------------------------------------------------------------//
MOAIMaterial::MOAIMaterial () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIMaterial::~MOAIMaterial () {
	this->mShader.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIMaterial::RegisterLuaClass ( MOAILuaState& state ) {

	MOAINode::RegisterLuaClass ( state );

	state.SetField ( -1, "VALUETYPE_TEXTURE",			( u32 )MOAIMaterial::VALUETYPE_TEXTURE );
	state.SetField ( -1, "VALUETYPE_FLOAT",				( u32 )MOAIMaterial::VALUETYPE_FLOAT );
	state.SetField ( -1, "VALUETYPE_VEC2",				( u32 )MOAIMaterial::VALUETYPE_VEC2 );
	state.SetField ( -1, "VALUETYPE_VEC3",				( u32 )MOAIMaterial::VALUETYPE_VEC3 );
	state.SetField ( -1, "VALUETYPE_VEC4",				( u32 )MOAIMaterial::VALUETYPE_VEC4 );

	state.SetField ( -1, "DYNAMICVALUE_SIMTIME",			( u32 )MOAIMaterial::DYNAMICVALUE_SIMTIME );
}

//----------------------------------------------------------------//
void MOAIMaterial::RegisterLuaFuncs ( MOAILuaState& state ) {

	MOAINode::RegisterLuaFuncs ( state );

	luaL_Reg regTable [] = {
		{ "setShader",			_setShader },
		{ "setShaderValue",		_setShaderValue },
		{ "setDynamicShaderValue",	_setDynamicShaderValue },
		{ "setTexture",			_setTexture },
		{ "setColor",			_setColor },
		{ "setCullMode",		_setCullMode },
		{ "setDepthTest",		_setDepthTest },
		{ "setDepthWrite",		_setDepthWrite },
		{ "setBlendMode",		_setBlendMode },
		{ "clearShaderValues",		_clearShaderValues },
		{ "clearDynamicShaderValues",	_clearDynamicShaderValues },
		{ "resetPermutationState",	_resetPermutationState },
		{ "setPermutationFlag",		_setPermutationFlag },
		{ "setPermutationSwitch",	_setPermutationSwitch },
		{ NULL, NULL }
	};
	luaL_register ( state, 0, regTable );
}


//----------------------------------------------------------------//
void MOAIMaterial::LoadGfxState()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get ();

	if (this->mShader)
		gfxDevice.SetShader(this->mShader);

	gfxDevice.SetBlendMode ( this->mBlendMode );
}
