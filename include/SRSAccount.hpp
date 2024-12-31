#include <pjsua2.hpp>
#include <iostream>


class SRSAccount : public pj::Account
{
public:
    /**
     * Notify application when registration or unregistration has been
     * initiated. Note that this only notifies the initial registration
     * and unregistration. Once registration session is active, subsequent
     * refresh will not cause this callback to be called.
     */
    void onRegState(pj::OnRegStateParam& param);
    
    /* Notify application on incoming call. */
    void onIncomingCall(pj::OnIncomingCallParam& incomingParam);
};