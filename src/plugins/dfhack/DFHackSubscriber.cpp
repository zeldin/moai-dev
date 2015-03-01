#include <moai_config.h>
#include <zl-util/headers.h>
#include <moai-core/headers.h>

#include <dfhack/DFHackSubscriber.h>

//----------------------------------------------------------------//

DFHackSubscriber::DFHackSubscriber ()
{
  RTTI_BEGIN
    RTTI_EXTEND ( MOAILuaObject )
  RTTI_END
}

//----------------------------------------------------------------//

DFHackSubscriber::~DFHackSubscriber ()
{
}

//----------------------------------------------------------------//

void DFHackSubscriber::RegisterLuaClass ( MOAILuaState& state )
{
  state.SetField(-1, "DOWNLOADING", DOWNLOADING);
  state.SetField(-1, "DOWNLOAD_FAILED", DOWNLOAD_FAILED);
  state.SetField(-1, "DOWNLOAD_SAVE_FAILED", DOWNLOAD_SAVE_FAILED);
  state.SetField(-1, "FAILED", FAILED);
  state.SetField(-1, "GETTING_SUBSCRIPTION_INFO", GETTING_SUBSCRIPTION_INFO);
  state.SetField(-1, "GET_SUBSCRIPTION_INFO_FAILED", GET_SUBSCRIPTION_INFO_FAILED);
  state.SetField(-1, "LOOKING_UP_SUBSCRIPTIONS", LOOKING_UP_SUBSCRIPTIONS);
  state.SetField(-1, "LOOKUP_SUBSCRIPTION_FAILED", LOOKUP_SUBSCRIPTION_FAILED);
  state.SetField(-1, "NONE", NONE);
  state.SetField(-1, "READY", READY);

  luaL_Reg regTable [] = {
    { NULL, NULL }
  };
  luaL_register ( state, 0, regTable );
}

