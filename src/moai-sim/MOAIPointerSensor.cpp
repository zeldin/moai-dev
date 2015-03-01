// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIPointerSensor.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@name	getLoc
	@text	Returns the location of the pointer on the screen.

	@in		MOAIPointerSensor self
	@out	number x
	@out	number y
*/
int MOAIPointerSensor::_getLoc ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPointerSensor, "U" )

	lua_pushnumber ( state, self->mX );
	lua_pushnumber ( state, self->mY );

	return 2;
}

//----------------------------------------------------------------//
/**	@name	setCallback
	@text	Sets or clears the callback to be issued when the pointer location changes.

	@in		MOAIPointerSensor self
	@opt	function callback			Default value is nil.
	@out	nil
*/
int MOAIPointerSensor::_setCallback ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPointerSensor, "U" )
	
	self->mOnMove.SetRef ( state, 2 );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIPointerSensor::_show( lua_State* L )
{
	MOAI_LUA_SETUP ( MOAIPointerSensor, "UB" )
	bool show = state.GetValue < bool >( 2, true );

	return 0;
}

//================================================================//
// MOAIPointerSensor
//================================================================//

//----------------------------------------------------------------//
void MOAIPointerSensor::HandleEvent ( ZLStream& eventStream ) {

	int x = eventStream.Read < int >( 0 );
	int y = eventStream.Read < int >( 0 );

	this->mX = x;
	this->mY = y;
	
	if ( this->mOnMove ) {
		MOAIScopedLuaState state = this->mOnMove.GetSelf ();
		lua_pushnumber ( state, this->mX );
		lua_pushnumber ( state, this->mY );
		state.DebugCall ( 2, 0 );
	}
}

//----------------------------------------------------------------//
MOAIPointerSensor::MOAIPointerSensor () {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAIPointerSensor::~MOAIPointerSensor () {
}

//----------------------------------------------------------------//
void MOAIPointerSensor::RegisterLuaClass ( MOAILuaState& state ) {

	MOAISensor::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAIPointerSensor::RegisterLuaFuncs ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "getLoc",			_getLoc },
		{ "setCallback",	_setCallback },
		{ "show", _show },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIPointerSensor::WriteEvent ( ZLStream& eventStream, int x, int y ) {

	eventStream.Write < int >( x );
	eventStream.Write < int >( y );
}
