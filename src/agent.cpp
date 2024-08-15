#include <pjsua2.hpp>
#include <iostream>
#include <csignal>

using namespace pj;

bool isShutdown = false;


class MyCall : public Call {
    public:
        AudioMediaRecorder recorder;
        
        MyCall(Account &account, int callID = PJSUA_INVALID_ID)
        : Call(account, callID)
        { }

        void printCallState(string state, string localUri, string remoteUri, long connectDuration, string callID){
            std::cout << "########## " << "Call-ID:" << callID;
            std::cout << "\t";
            std::cout << "State:" << state;
            std::cout << "\t";
            std::cout << "(" << remoteUri << " -> " << localUri << ")";
            std::cout << "\t";
            std::cout << "Duration:" << connectDuration << "sec";
            std::cout << std::endl;
        }

        string getWavFileName(){
            CallInfo callInfo = getInfo();
            return callInfo.callIdString + ".wav";
        }

        void saveAudioMedio(AudioMedia audioMedio){
            string fileName = getWavFileName();
            recorder.createRecorder(fileName);
            audioMedio.startTransmit(recorder);
        }

        void echoVoice(AudioMedia audioMedia){
            AudDevManager& manager = Endpoint::instance().audDevManager();
            audioMedia.startTransmit(manager.getPlaybackDevMedia());
            manager.getCaptureDevMedia().startTransmit(audioMedia);
        }

        virtual void onCallState(OnCallStateParam &param){
            CallInfo callInfo = getInfo();
            if (callInfo.state == PJSIP_INV_STATE_CONNECTING || callInfo.state == PJSIP_INV_STATE_DISCONNECTED){
                printCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri, callInfo.connectDuration.sec, callInfo.callIdString);
            }
        }
  
        virtual void onCallMediaState(OnCallMediaStateParam &params)
        {
            CallInfo callInfo = getInfo();
            for (unsigned i = 0; i < callInfo.media.size(); i++)
            {
                if (callInfo.media[i].type == PJMEDIA_TYPE_AUDIO)
                {
                    AudioMedia audioMedia = getAudioMedia(i);
                    saveAudioMedio(audioMedia);
                    echoVoice(audioMedia);
                }
            }
        }
};

// Subclass to extend the Account and get notifications etc.
class MyAccount : public Account {
    public:
        virtual void onRegState(OnRegStateParam &param) {
            AccountInfo accountInfo = getInfo();
            std::cout << (accountInfo.regIsActive? "*** Register:" : "*** Unregister:")
                    << " code=" << param.code << std::endl;
        }
        virtual void onIncomingCall(OnIncomingCallParam &incomingParam){
            MyCall *call = new MyCall(*this, incomingParam.callId);
            CallOpParam param;
            param.statusCode = PJSIP_SC_OK;
            call->answer(param);
        }
};


void signalCallbackHandler(int signum) {
    isShutdown = true;
}

int main()
{
    Endpoint endpoint;

    endpoint.libCreate();

    // Initialize endpoint
    EpConfig endpointConfig;
    endpoint.libInit(endpointConfig);

    // Create SIP transport. Error handling sample is shown
    TransportConfig transportConfig;
    transportConfig.port = 6060;
    try {
        endpoint.transportCreate(PJSIP_TRANSPORT_UDP, transportConfig);
    } catch (Error &err) {
        std::cout << err.info() << std::endl;
        return 1;
    }

    // Start the library (worker threads etc)
    endpoint.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    // Configure an AccountConfig
    AccountConfig accountConfig;
    accountConfig.idUri = "sip:1000@192.168.228.130:5060";
    accountConfig.regConfig.registrarUri = "sip:192.168.228.130:5060";

    // Create the account
    MyAccount *account = new MyAccount;
    account->create(accountConfig);

    signal(SIGINT, signalCallbackHandler);
    while(!isShutdown){
       sleep(1);
    }

    // Delete the account. This will unregister from server
    delete account;

    return 0;
}