#include <iostream>

#include <pjsua2.hpp>

class SRSAccount : public pj::Account {
public:
  /* Notify application on incoming call. */
  void onIncomingCall(pj::OnIncomingCallParam& incoming_param) override;
};