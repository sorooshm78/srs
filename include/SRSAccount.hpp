#include <pjsua2.hpp>
#include <iostream>


class SRSAccount : public pj::Account
{
public:
    void onRegState(pj::OnRegStateParam& param);
    void onIncomingCall(pj::OnIncomingCallParam& incomingParam);

};