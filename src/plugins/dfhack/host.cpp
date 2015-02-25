#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>

#include <dfhack/host.h>
#include <dfhack/DFHack.h>

static bool sIsInitialized = false;

//----------------------------------------------------------------//

void DFHackAppFinalize () {
  if ( !sIsInitialized ) return;
  sIsInitialized = false;
}

void DFHackAppInitialize () {

}

//----------------------------------------------------------------//

void DFHackContextInitialize () {
  if ( !sIsInitialized ) {
    sIsInitialized = true;
  }
  REGISTER_LUA_CLASS ( DFHack )
}

