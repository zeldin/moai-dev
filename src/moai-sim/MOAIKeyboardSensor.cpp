// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIKeyboardSensor.h>

//================================================================//
// MOAIKeyboardEvent
//================================================================//
class MOAIKeyboardEvent {
public:

	u32		mKey;
	bool	mDown;
};

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@name	keyDown
	@text	Checks to see if one or more buttons were pressed during the last iteration.

	@overload
	@in		MOAIKeyboardSensor self
	@in		string keyCodes			Keycode value(s) to be checked against the input table.
	@out	boolean... wasPressed

	@overload
	@in		MOAIKeyboardSensor self
	@in		number keyCode			Keycode value to be checked against the input table.
	@out	boolean wasPressed
*/
int MOAIKeyboardSensor::_keyDown ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIKeyboardSensor, "U" )
	
	if ( state.IsType ( 2, LUA_TSTRING )) {
		
		cc8* str = lua_tostring ( state, 2 );
		
		int count;
		for ( count = 0; str [ count ]; ++count ) {
			lua_pushboolean ( state, self->KeyDown ( str [ count ]));
		}
		return count;
	}
	
	if ( state.IsType ( 2, LUA_TNUMBER )) {
	
		u32 keyCode = state.GetValue < u32 >( 2, 0 );
		lua_pushboolean ( state, self->KeyDown ( keyCode ));
		return 1;
	}
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	keyIsDown
	@text	Checks to see if the button is currently down.

	@overload
	@in		MOAIKeyboardSensor self
	@in		string keyCodes			Keycode value(s) to be checked against the input table.
	@out	boolean... isDown

	@overload
	@in		MOAIKeyboardSensor self
	@in		number keyCode			Keycode value to be checked against the input table.
	@out	boolean isDown
*/
int MOAIKeyboardSensor::_keyIsDown ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIKeyboardSensor, "U" )
	
	if ( state.IsType ( 2, LUA_TSTRING )) {
		
		cc8* str = lua_tostring ( state, 2 );
		
		int count;
		for ( count = 0; str [ count ]; ++count ) {
			lua_pushboolean ( state, self->KeyIsDown ( str [ count ]));
		}
		return count;
	}
	
	if ( state.IsType ( 2, LUA_TNUMBER )) {
	
		u32 keyCode = state.GetValue < u32 >( 2, 0 );
		lua_pushboolean ( state, self->KeyIsDown ( keyCode ));
		return 1;
	}
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	keyIsUp
	@text	Checks to see if the specified key is currently up.

	@overload
	@in		MOAIKeyboardSensor self
	@in		string keyCodes			Keycode value(s) to be checked against the input table.
	@out	boolean... isUp

	@overload
	@in		MOAIKeyboardSensor self
	@in		number keyCode			Keycode value to be checked against the input table.
	@out	boolean isUp
*/
int MOAIKeyboardSensor::_keyIsUp ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIKeyboardSensor, "U" )
	
	if ( state.IsType ( 2, LUA_TSTRING )) {
		
		cc8* str = lua_tostring ( state, 2 );
		
		int count;
		for ( count = 0; str [ count ]; ++count ) {
			lua_pushboolean ( state, self->KeyIsUp ( str [ count ]));
		}
		return count;
	}
	
	if ( state.IsType ( 2, LUA_TNUMBER )) {
	
		u32 keyCode = state.GetValue < u32 >( 2, 0 );
		lua_pushboolean ( state, self->KeyIsUp ( keyCode ));
		return 1;
	}
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	keyUp
	@text	Checks to see if the specified key was released during the last iteration.

	@overload
	@in		MOAIKeyboardSensor self
	@in		string keyCodes			Keycode value(s) to be checked against the input table.
	@out	boolean... wasReleased

	@overload
	@in		MOAIKeyboardSensor self
	@in		number keyCode			Keycode value to be checked against the input table.
	@out	boolean wasReleased
*/
int MOAIKeyboardSensor::_keyUp ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIKeyboardSensor, "U" )
	
	if ( state.IsType ( 2, LUA_TSTRING )) {
		
		cc8* str = lua_tostring ( state, 2 );
		
		int count;
		for ( count = 0; str [ count ]; ++count ) {
			lua_pushboolean ( state, self->KeyUp ( str [ count ]));
		}
		return count;
	}
	
	if ( state.IsType ( 2, LUA_TNUMBER )) {
	
		u32 keyCode = state.GetValue < u32 >( 2, 0 );
		lua_pushboolean ( state, self->KeyUp ( keyCode ));
		return 1;
	}
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setCallback
	@text	Sets or clears the callback to be issued when a key is pressed.

	@in		MOAIKeyboardSensor self
	@opt	function callback			Default value is nil.
	@out	nil
*/
int MOAIKeyboardSensor::_setCallback ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIKeyboardSensor, "U" )
	
	self->mOnKey.SetRef ( state, 2 );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setTextCallback
	@text	Sets or clears the callback to be issued when text is input.

	@in		MOAIKeyboardSensor self
	@opt	function callback			Default value is nil.
	@out	nil
*/
int MOAIKeyboardSensor::_setTextCallback ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIKeyboardSensor, "U" )

	self->mOnText.SetRef ( state, 2 );

	return 0;
}

//================================================================//
// MOAIKeyboardSensor
//================================================================//

//----------------------------------------------------------------//
void MOAIKeyboardSensor::HandleEvent ( ZLStream& eventStream ) {
	
	u32 keyCode = eventStream.Read < u32 >( 0 );
	bool down = eventStream.Read < bool >( false );
	
	bool inQueue = (( this->mState [ keyCode ] & ( DOWN | UP )) != 0 );
	
	if ( down ) {
		this->mState [ keyCode ] |= IS_DOWN | DOWN;
	}
	else {
		this->mState [ keyCode ] &= ~IS_DOWN;
		this->mState [ keyCode ] |= UP;
	}
	
	if ( this->mOnKey ) {
		MOAIScopedLuaState state = this->mOnKey.GetSelf ();
		lua_pushnumber ( state, keyCode );
		lua_pushboolean ( state, down );
		state.DebugCall ( 2, 0 );
	}
	
	if ( !inQueue ) {
		this->mClearQueue [ this->mClearCount ] = keyCode;
		this->mClearCount++;
	}
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyDown ( u32 keyID ) {

	return (( this->mState [ keyID ] & DOWN ) == DOWN );
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyIsDown ( u32 keyID ) {

	return (( this->mState [ keyID ] & IS_DOWN ) == IS_DOWN );
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyIsUp ( u32 keyID ) {

	return (( this->mState [ keyID ] & IS_DOWN ) == 0 );
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyUp ( u32 keyID ) {

	return (( this->mState [ keyID ] & UP ) == UP );
}

//----------------------------------------------------------------//
MOAIKeyboardSensor::MOAIKeyboardSensor () :
	mClearCount ( 0 ) {
	
	RTTI_SINGLE ( MOAISensor )
	
	memset ( this->mState, 0, MOAIKeyCodes::TOTAL * sizeof ( u32 ));
}

//----------------------------------------------------------------//
MOAIKeyboardSensor::~MOAIKeyboardSensor () {
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::RegisterLuaClass ( MOAILuaState& state ) {

	MOAISensor::RegisterLuaClass ( state );

	state.SetField ( -1, "SHIFT", ( u32 )MOAIKeyCodes::SHIFT );
	state.SetField ( -1, "CONTROL", ( u32 )MOAIKeyCodes::CONTROL );
	state.SetField ( -1, "ALT", ( u32 )MOAIKeyCodes::ALT );

	state.SetField ( -1, "UP", ( u32 )MOAIKeyCodes::UP );
	state.SetField ( -1, "DOWN", ( u32 )MOAIKeyCodes::DOWN );
	state.SetField ( -1, "RIGHT", ( u32 )MOAIKeyCodes::RIGHT );
	state.SetField ( -1, "LEFT", ( u32 )MOAIKeyCodes::LEFT );
	state.SetField ( -1, "INSERT", ( u32 )MOAIKeyCodes::INSERT );
	state.SetField ( -1, "HOME", ( u32 )MOAIKeyCodes::HOME );
	state.SetField ( -1, "END", ( u32 )MOAIKeyCodes::END );
	state.SetField ( -1, "PAGE_UP", ( u32 )MOAIKeyCodes::PAGE_UP );
	state.SetField ( -1, "PAGE_DOWN", ( u32 )MOAIKeyCodes::PAGE_DOWN );
	state.SetField ( -1, "F1", ( u32 )MOAIKeyCodes::F1 );
	state.SetField ( -1, "F2", ( u32 )MOAIKeyCodes::F2 );
	state.SetField ( -1, "F3", ( u32 )MOAIKeyCodes::F3 );
	state.SetField ( -1, "F4", ( u32 )MOAIKeyCodes::F4 );
	state.SetField ( -1, "F5", ( u32 )MOAIKeyCodes::F5 );
	state.SetField ( -1, "F6", ( u32 )MOAIKeyCodes::F6 );
	state.SetField ( -1, "F7", ( u32 )MOAIKeyCodes::F7 );
	state.SetField ( -1, "F8", ( u32 )MOAIKeyCodes::F8 );
	state.SetField ( -1, "F9", ( u32 )MOAIKeyCodes::F9 );
	state.SetField ( -1, "F10", ( u32 )MOAIKeyCodes::F10 );
	state.SetField ( -1, "F11", ( u32 )MOAIKeyCodes::F11 );
	state.SetField ( -1, "F12", ( u32 )MOAIKeyCodes::F12 );
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::RegisterLuaFuncs ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "keyDown",				_keyDown },
		{ "keyIsDown",				_keyIsDown },
		{ "keyIsUp",				_keyIsUp },
		{ "keyUp",					_keyUp },
		{ "setCallback",			_setCallback },
		{ "setTextCallback",			_setTextCallback },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::Reset () {

	// clear out the old events
	for ( u32 i = 0; i < this->mClearCount; ++i ) {
		u32 keyCode = this->mClearQueue [ i ];
		this->mState [ keyCode ] &= ~( DOWN | UP );	
	}
	this->mClearCount = 0;
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::WriteEvent ( ZLStream& eventStream, u32 key, bool down ) {

	eventStream.Write < u32 >( key );
	eventStream.Write < bool >( down );
}
