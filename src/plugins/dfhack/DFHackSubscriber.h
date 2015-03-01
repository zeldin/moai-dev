#ifndef DFHACKSUBSCRIBER_H
#define DFHACKSUBSCRIBER_H

class DFHackSubscriber :
public virtual MOAILuaObject {
private:
	  enum {
	    DOWNLOADING = 1,
	    DOWNLOAD_FAILED,
	    DOWNLOAD_SAVE_FAILED,
	    FAILED,
	    GETTING_SUBSCRIPTION_INFO,
	    GET_SUBSCRIPTION_INFO_FAILED,
	    LOOKING_UP_SUBSCRIPTIONS,
	    LOOKUP_SUBSCRIPTION_FAILED,
	    NONE,
	    READY
	  };
public:
	  DECL_LUA_FACTORY ( DFHackSubscriber )

	  //----------------------------------------------------------------//

	  DFHackSubscriber	();
	  ~DFHackSubscriber	();
	  void	RegisterLuaClass	( MOAILuaState& state );
};
#endif
