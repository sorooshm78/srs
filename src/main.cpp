#include <pjsua2.hpp>
#include <iostream>
#include <csignal>

using namespace pj;

bool isShutdown = false;
const int LISTEN_PORT = 5060;
const string USER = "1010";


enum loglevel
{
    FATAL_ERROR = 0,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE,
    DETAILED_TRACE,
};

class MyCall : public Call
{
public:
    AudioMediaRecorder recorder1;
    AudioMediaRecorder recorder2;

    MyCall(Account& account, int callID = PJSUA_INVALID_ID)
        : Call(account, callID)
    {
    }

    void printCallState(string state, string localUri, string remoteUri, long connectDuration, string callID)
    {
        std::cout << "########## " << "Call-ID:" << callID;
        std::cout << "\t";
        std::cout << "State:" << state;
        std::cout << "\t";
        std::cout << "(" << remoteUri << " -> " << localUri << ")";
        std::cout << "\t";
        std::cout << "Duration:" << connectDuration << "sec";
        std::cout << std::endl;
    }

    string getWavFileName(int media_index)
    {
        CallInfo callInfo = getInfo();
        int recorder = media_index + 1;
        return callInfo.callIdString + "-" + std::to_string(recorder) +  ".wav";
    }

    void saveAudioMedia(AudioMedia audioMedio, int media_index)
    {
        string fileName = getWavFileName(media_index);
        if(media_index == 0){
            recorder1.createRecorder(fileName);
            audioMedio.startTransmit(recorder1);
        }
        if(media_index == 1){
            recorder2.createRecorder(fileName);
            audioMedio.startTransmit(recorder2);
        }
    }

    virtual void onCallState(OnCallStateParam& param)
    {
        CallInfo callInfo = getInfo();
        if (callInfo.state == PJSIP_INV_STATE_CONNECTING || callInfo.state == PJSIP_INV_STATE_DISCONNECTED)
        {
            printCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri, callInfo.connectDuration.sec,
                           callInfo.callIdString);
        }
    }

    virtual void onCallMediaState(OnCallMediaStateParam& params)
    {
        CallInfo callInfo = getInfo();
        for (unsigned media_index = 0; media_index < callInfo.media.size(); media_index++)
        {
            if (callInfo.media[media_index].type == PJMEDIA_TYPE_AUDIO)
            {
                AudioMedia audioMedia = getAudioMedia(media_index);
                saveAudioMedia(audioMedia, media_index);
            }
        }
    }
};


class MyAccount : public Account
{
public:
    void onRegState(OnRegStateParam& param) override
    {
        AccountInfo accountInfo = getInfo();
        std::cout << (accountInfo.regIsActive ? "*** Register:" : "*** Unregister:")
            << " code=" << param.code << std::endl;
    }

    void onIncomingCall(OnIncomingCallParam& incomingParam) override
    {
        MyCall* call = new MyCall(*this, incomingParam.callId);
        CallOpParam param;
        param.statusCode = PJSIP_SC_OK;
        call->answer(param);
    }
};


void signalCallbackHandler(int signum)
{
    isShutdown = true;
}

int main(int argc, char* argv[])
{
    Endpoint endpoint;
    endpoint.libCreate();

    EpConfig endpointConfig;
    endpointConfig.logConfig.level = loglevel::WARNING;
    endpointConfig.logConfig.consoleLevel = loglevel::WARNING;
    endpoint.libInit(endpointConfig);
    endpoint.audDevManager().setNullDev();

    TransportConfig transportConfig;
    transportConfig.port = LISTEN_PORT;
    try
    {
        endpoint.transportCreate(PJSIP_TRANSPORT_UDP, transportConfig);
    }
    catch (Error& err)
    {
        std::cout << err.info() << std::endl;
        return 1;
    }

    endpoint.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    AccountConfig accountConfig;
    accountConfig.idUri = "sip:" + USER + "@192.168.21.88";
    accountConfig.regConfig.registrarUri = "";
    accountConfig.sipConfig.authCreds.clear();

    // Create the account
    auto* account = new MyAccount;
    account->create(accountConfig);

    signal(SIGINT, signalCallbackHandler);
    while (!isShutdown)
    {
        sleep(1);
    }

    delete account;

    return 0;
}
