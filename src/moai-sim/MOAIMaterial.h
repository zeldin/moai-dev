#ifndef	MOAIMATERIAL_H
#define	MOAIMATERIAL_H

#include <moai-sim/MOAINode.h>
#include <moai-sim/MOAIBlendMode.h>
class MOAIShader;
class MOAITextureBase;
class MOAIMultiTexture;

class MOAIMaterial :
	public virtual MOAINode {
protected:
	MOAILuaSharedPtr < MOAIShader >			mShader;
	MOAIBlendMode							mBlendMode;
	typedef STLMap < u32, MOAIAttrOp >::iterator ShaderValuesIt;
	STLMap < u32, MOAIAttrOp > mShaderValues;
	MOAILuaSharedPtr < MOAIMultiTexture > mMultiTexture;
	u32 mTexturesUsed;

	std::map<u32,MOAIAttrOp> m_ShaderValues;
	//----------------------------------------------------------------//
	static int		_setShader		( lua_State* L );
	static int		_setShaderValue		( lua_State* L );
	static int		_setDynamicShaderValue	( lua_State* L );
	static int		_setTexture		( lua_State* L );
	static int		_setColor		( lua_State* L );
	static int		_setCullMode		( lua_State* L );
	static int		_setDepthTest		( lua_State* L );
	static int		_setDepthWrite		( lua_State* L );
	static int		_setBlendMode		( lua_State* L );
	static int		_clearShaderValues	( lua_State* L );
	static int		_clearDynamicShaderValues( lua_State* L );
	static int		_resetPermutationState	( lua_State* L );
	static int		_setPermutationFlag	( lua_State* L );
	static int		_setPermutationSwitch	( lua_State* L );
	//----------------------------------------------------------------//

	enum {
	  VALUETYPE_NONE,
	  VALUETYPE_TEXTURE = 100,
	  VALUETYPE_FLOAT,
	  VALUETYPE_VEC2,
	  VALUETYPE_VEC3,
	  VALUETYPE_VEC4,
	};

	enum {
	  DYNAMICVALUE_SIMTIME = 200,
	};

public:

	DECL_LUA_FACTORY ( MOAIMaterial )

	MOAIMaterial				();
	~MOAIMaterial				();
	void	RegisterLuaClass		( MOAILuaState& state );
	void	RegisterLuaFuncs		( MOAILuaState& state );

	//----------------------------------------------------------------//
	void	LoadGfxState	();
};

#endif
