#include "SRSAccount.hpp"
#include "SRSCall.hpp"
#include <pjsua2.hpp>
#include <iostream>


void SRSAccount::onIncomingCall(pj::OnIncomingCallParam& incomingParam)
{
    SRSCall* call = new SRSCall(*this, incomingParam.callId);
    pj::CallOpParam param;
    param.statusCode = PJSIP_SC_OK;
    param.opt.audioCount = 2;
    param.opt.videoCount = 0;
    call->answer(param);
}
