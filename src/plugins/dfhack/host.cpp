#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>
#include <moai-sim/headers.h>

#include <dfhack/host.h>
#include <dfhack/DFHack.h>
#include <dfhack/DFHackSubscriber.h>

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
  REGISTER_LUA_CLASS ( DFHackSubscriber )

  char buf[PATH_MAX+1];
  ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
  if (len > 0) {
    buf[len] = 0;
    char *p = strrchr(buf, '/');
    if (p && p != buf) {
      *p = 0;
      zl_chdir(buf);
    }
  }

  char *docDir = getenv("XDG_DATA_HOME");
  if (docDir == NULL) {
    const char *home = getenv("HOME");
    if (home == NULL)
      home = "/tmp";
    snprintf(buf, sizeof(buf), "%s/%s", home, ".local/share");
    docDir = buf;
  }
  MOAIEnvironment::Get ().SetValue ( MOAI_ENV_documentDirectory, docDir );

  AKURunScript ( "Data/Scripts/config.lua" );
}

