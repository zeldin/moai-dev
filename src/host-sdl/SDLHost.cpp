// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <moai-core/host.h>
#include <host-modules/aku_modules.h>
#include <host-sdl/SDLHost.h>
#include <moai-sim/MOAIKeyCodes.h>

#include <SDL.h>

#define UNUSED(p) (( void )p)

namespace InputDeviceID {
	enum {
		DEVICE,
		TOTAL,
	};
}

namespace InputSensorID {
	enum {
		KEYBOARD,
		POINTER,
		MOUSE_LEFT,
		MOUSE_MIDDLE,
		MOUSE_RIGHT,
		MOUSE_SCROLL_UP,
		MOUSE_SCROLL_DOWN,
		TOUCH,
		GAMEPAD_STICK_LEFT,
		GAMEPAD_BUTTON_A,
		GAMEPAD_BUTTON_B,
		GAMEPAD_BUTTON_X,
		GAMEPAD_BUTTON_Y,
		GAMEPAD_BUTTON_DPAD_UP,
		GAMEPAD_BUTTON_DPAD_DOWN,
		GAMEPAD_BUTTON_DPAD_LEFT,
		GAMEPAD_BUTTON_DPAD_RIGHT,
		GAMEPAD_BUTTON_L_SHOULDER,
		GAMEPAD_BUTTON_R_SHOULDER,
		GAMEPAD_BUTTON_START,
		GAMEPAD_BUTTON_BACK,
		TOTAL,
	};
}

static SDL_Window* sWindow = 0;

//================================================================//
// aku callbacks
//================================================================//

void	_AKUEnterFullscreenModeFunc		();
void	_AKUExitFullscreenModeFunc		();
void	_AKUOpenWindowFunc				( const char* title, int width, int height );

//----------------------------------------------------------------//
void _AKUEnterFullscreenModeFunc () {

	printf ( "UNSUPPORTED\n" );
}

//----------------------------------------------------------------//
void _AKUExitFullscreenModeFunc () {

	printf ( "UNSUPPORTED\n" );
}

//----------------------------------------------------------------//
void _AKUOpenWindowFunc ( const char* title, int width, int height ) {
	
	if ( !sWindow ) {
		sWindow = SDL_CreateWindow ( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		SDL_GL_CreateContext ( sWindow );
		SDL_GL_SetSwapInterval ( 1 );
		AKUDetectGfxContext ();
		AKUSetScreenSize ( width, height );
	}
}

//================================================================//
// helpers
//================================================================//

static void	Finalize			();
static void	Init				( int argc, char** argv );
static int	RemapScancode			( int key );
static void	MainLoop			();
static void	PrintMoaiVersion	();

//----------------------------------------------------------------//
void Finalize () {

	AKUModulesAppFinalize ();
	AKUAppFinalize ();
	
	SDL_Quit ();
}

//----------------------------------------------------------------//
void Init ( int argc, char** argv ) {

	SDL_Init ( SDL_INIT_EVERYTHING );
	PrintMoaiVersion ();

	#ifdef _DEBUG
		printf ( "DEBUG BUILD\n" );
	#endif

	AKUAppInitialize ();
	AKUModulesAppInitialize ();

	AKUCreateContext ();
	AKUModulesContextInitialize ();
	AKUModulesRunLuaAPIWrapper ();

	AKUSetInputConfigurationName ( "SDL" );

	AKUReserveInputDevices			( InputDeviceID::TOTAL );
	AKUSetInputDevice				( InputDeviceID::DEVICE, "device" );
	
	AKUReserveInputDeviceSensors	( InputDeviceID::DEVICE, InputSensorID::TOTAL );
	AKUSetInputDeviceKeyboard		( InputDeviceID::DEVICE, InputSensorID::KEYBOARD,		"keyboard" );
	AKUSetInputDevicePointer		( InputDeviceID::DEVICE, InputSensorID::POINTER,		"pointer" );
	AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputSensorID::MOUSE_LEFT,		"mouseLeft" );
	AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputSensorID::MOUSE_MIDDLE,	"mouseMiddle" );
	AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputSensorID::MOUSE_RIGHT,	"mouseRight" );
	AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputSensorID::MOUSE_SCROLL_UP,	"mouseScrollUp" );
	AKUSetInputDeviceButton			( InputDeviceID::DEVICE, InputSensorID::MOUSE_SCROLL_DOWN,	"mouseScrollDown" );

	AKUSetInputDeviceJoystick ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_STICK_LEFT, "gamepadStickLeft" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_A, "gamepadButtonA" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_B, "gamepadButtonB" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_X, "gamepadButtonX" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_Y, "gamepadButtonY" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_DPAD_UP, "gamepadButtonDPadUp" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_DPAD_DOWN, "gamepadButtonDPadDown" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_DPAD_LEFT, "gamepadButtonDPadLeft" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_DPAD_RIGHT, "gamepadButtonDPadRight" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_L_SHOULDER, "gamepadButtonLShoulder" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_R_SHOULDER, "gamepadButtonRShoulder" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_START, "gamepadButtonStart" );
	AKUSetInputDeviceButton   ( InputDeviceID::DEVICE, InputSensorID::GAMEPAD_BUTTON_BACK, "gamepadButtonBack" );

	AKUSetFunc_EnterFullscreenMode ( _AKUEnterFullscreenModeFunc );
	AKUSetFunc_ExitFullscreenMode ( _AKUExitFullscreenModeFunc );
	AKUSetFunc_OpenWindow ( _AKUOpenWindowFunc );
	
	AKUModulesParseArgs ( argc, argv );
	
	atexit ( Finalize ); // do this *after* SDL_Init
}

//----------------------------------------------------------------//
int RemapScancode (int key) {
	switch (key) {
	case SDL_SCANCODE_F1: return MOAIKeyCodes::F1;
	case SDL_SCANCODE_F2: return MOAIKeyCodes::F2;
	case SDL_SCANCODE_F3: return MOAIKeyCodes::F3;
	case SDL_SCANCODE_F4: return MOAIKeyCodes::F4;
	case SDL_SCANCODE_F5: return MOAIKeyCodes::F5;
	case SDL_SCANCODE_F6: return MOAIKeyCodes::F6;
	case SDL_SCANCODE_F7: return MOAIKeyCodes::F7;
	case SDL_SCANCODE_F8: return MOAIKeyCodes::F8;
	case SDL_SCANCODE_F9: return MOAIKeyCodes::F9;
	case SDL_SCANCODE_F10: return MOAIKeyCodes::F10;
	case SDL_SCANCODE_F11: return MOAIKeyCodes::F11;
	case SDL_SCANCODE_F12: return MOAIKeyCodes::F12;
	case SDL_SCANCODE_LEFT: return MOAIKeyCodes::LEFT;
	case SDL_SCANCODE_UP: return MOAIKeyCodes::UP;
	case SDL_SCANCODE_RIGHT: return MOAIKeyCodes::RIGHT;
	case SDL_SCANCODE_DOWN: return MOAIKeyCodes::DOWN;
	case SDL_SCANCODE_PAGEUP: return MOAIKeyCodes::PAGE_UP;
	case SDL_SCANCODE_PAGEDOWN: return MOAIKeyCodes::PAGE_DOWN;
	case SDL_SCANCODE_HOME: return MOAIKeyCodes::HOME;
	case SDL_SCANCODE_END: return MOAIKeyCodes::END;
	case SDL_SCANCODE_INSERT: return MOAIKeyCodes::INSERT;
	}
	return 0;
}

//----------------------------------------------------------------//
void MainLoop () {

	Uint32 lastFrame = SDL_GetTicks();
	
	bool running = true;
	while ( running ) {
	
		SDL_Event sdlEvent;
		
		while ( SDL_PollEvent ( &sdlEvent )) {  
			   
			switch ( sdlEvent.type )  {
			
				case SDL_QUIT:
				
					running = false;
					break;
				
				case SDL_KEYDOWN:
				case SDL_KEYUP:	{
					int key = sdlEvent.key.keysym.sym;
					if (key & 0x40000000) key = RemapScancode (key & 0x3FFFFFFF);

					if (key > 0 && key < MOAIKeyCodes::TOTAL)
						AKUEnqueueKeyboardEvent ( InputDeviceID::DEVICE, InputSensorID::KEYBOARD, key, sdlEvent.key.type == SDL_KEYDOWN );

				} 	break;
					
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
	
					switch ( sdlEvent.button.button ) {
					
						case SDL_BUTTON_LEFT:
							AKUEnqueueButtonEvent ( InputDeviceID::DEVICE, InputSensorID::MOUSE_LEFT, ( sdlEvent.type == SDL_MOUSEBUTTONDOWN ));
							break;
						
						case SDL_BUTTON_MIDDLE:
							AKUEnqueueButtonEvent ( InputDeviceID::DEVICE, InputSensorID::MOUSE_MIDDLE, ( sdlEvent.type == SDL_MOUSEBUTTONDOWN ));
							break;
						
						case SDL_BUTTON_RIGHT:
							AKUEnqueueButtonEvent ( InputDeviceID::DEVICE, InputSensorID::MOUSE_RIGHT, ( sdlEvent.type == SDL_MOUSEBUTTONDOWN ));
							break;
					}

					break;

				case SDL_MOUSEMOTION:
				
					AKUEnqueuePointerEvent ( InputDeviceID::DEVICE, InputSensorID::POINTER, sdlEvent.motion.x, sdlEvent.motion.y );
					break;
			}
		}
		
		AKUModulesUpdate ();
		
		AKURender ();
		SDL_GL_SwapWindow ( sWindow );
		
		Uint32 frameDelta = ( Uint32 )( AKUGetSimStep () * 1000.0 );
		Uint32 currentFrame = SDL_GetTicks ();
		Uint32 delta = currentFrame - lastFrame;
		
		if ( delta < frameDelta ) {
			SDL_Delay ( frameDelta - delta );
		}
		lastFrame = SDL_GetTicks();
	}
}

//----------------------------------------------------------------//
void PrintMoaiVersion () {

	static const int length = 255;
	char version [ length ];
	AKUGetMoaiVersion ( version, length );
	printf ( "%s\n", version );
}

//================================================================//
// SDLHost
//================================================================//

//----------------------------------------------------------------//
int SDLHost ( int argc, char** argv ) {

	Init ( argc, argv );

	if ( sWindow ) {
		MainLoop ();
	}

	return 0;
}
