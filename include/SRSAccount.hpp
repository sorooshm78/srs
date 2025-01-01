#include <pjsua2.hpp>
#include <iostream>


class SRSAccount : public pj::Account
{
public:
    /* Notify application on incoming call. */
    void onIncomingCall(pj::OnIncomingCallParam& incomingParam);
};