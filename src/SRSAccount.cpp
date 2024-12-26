#include "SRSAccount.hpp"
#include "SRSCall.hpp"
#include <pjsua2.hpp>
#include <iostream>


using namespace std;
using namespace pj;

void SRSAccount::onRegState(OnRegStateParam& param)
{
    AccountInfo accountInfo = getInfo();
    cout << (accountInfo.regIsActive ? "*** Register:" : "*** Unregister:")
        << " code=" << param.code << endl;
}

void  SRSAccount::onIncomingCall(OnIncomingCallParam& incomingParam)
{
    SRSCall* call = new SRSCall(*this, incomingParam.callId);
    CallOpParam param;
    param.statusCode = PJSIP_SC_OK;
    param.opt.audioCount = 2;
    param.opt.videoCount = 0;
    call->answer(param);
}
