#include "SRSAccount.hpp"
#include "SRSCall.hpp"

#include <iostream>

#include <pjsua2.hpp>
#include <gsl/gsl>

using pj::CallOpParam;
using pj::OnIncomingCallParam;

void SRSAccount::onIncomingCall(OnIncomingCallParam& incoming_param) {
  SRSCall* call = new SRSCall(*this, incoming_param.callId);
  Expects(call != nullptr);
  CallOpParam param;
  param.statusCode = PJSIP_SC_OK;
  param.opt.audioCount = 2;
  param.opt.videoCount = 0;
  call->answer(param);
}
