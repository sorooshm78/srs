#include "SRSAccount.hpp"
#include "SRSCall.hpp"
#include <pjsua2.hpp>
#include <gsl/gsl>
#include <iostream>


void SRSAccount::onIncomingCall(pj::OnIncomingCallParam& incomingParam)
{
    SRSCall* call = new SRSCall(*this, incomingParam.callId);
    Expects(call != nullptr);
    pj::CallOpParam param;
    param.statusCode = PJSIP_SC_OK;
    param.opt.audioCount = 2;
    param.opt.videoCount = 0;
    call->answer(param);
}
