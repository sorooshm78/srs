#include "SRSAccount.hpp"

#include <gsl/gsl>
#include <iostream>
#include <pjsua2.hpp>

#include "SRSCall.hpp"

void SRSAccount::onIncomingCall(pj::OnIncomingCallParam& incomingParam)
{
    auto* call = new SRSCall(*this, incomingParam.callId);
    Expects(call != nullptr);
    pj::CallOpParam param;
    param.statusCode = PJSIP_SC_OK;
    param.opt.audioCount = 2;
    param.opt.videoCount = 0;
    call->answer(param);
}
