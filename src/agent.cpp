#include <pjsua2.hpp>
#include <iostream>
#include <csignal>

using namespace pj;

bool is_shutdown = false;


class MyCall : public Call {
    public:
        MyCall(Account &acc, int call_id = PJSUA_INVALID_ID)
        : Call(acc, call_id)
        { }

        void print_call_state(string state, string localUri, string remoteUri, long connectDuration, string callID){
            std::cout << "########## " << "Call-ID:" << callID;
            std::cout << "\t";
            std::cout << "State:" << state;
            std::cout << "\t";
            std::cout << "(" << remoteUri << " -> " << localUri << ")";
            std::cout << "\t";
            std::cout << "Duration:" << connectDuration << "sec";
            std::cout << std::endl;
        }

        virtual void onCallState(OnCallStateParam &prm){
            CallInfo ci = getInfo();
            switch (ci.state)
            {
            case PJSIP_INV_STATE_INCOMING:
                print_call_state(ci.stateText, ci.localUri, ci.remoteUri, ci.connectDuration.sec, ci.callIdString);
                break;
            case PJSIP_INV_STATE_CONNECTING:
                print_call_state(ci.stateText, ci.localUri, ci.remoteUri, ci.connectDuration.sec, ci.callIdString);
                break;
            case PJSIP_INV_STATE_DISCONNECTED:
                print_call_state(ci.stateText, ci.localUri, ci.remoteUri, ci.connectDuration.sec, ci.callIdString);
                break;
            }
        }
};

// Subclass to extend the Account and get notifications etc.
class MyAccount : public Account {
    public:
        virtual void onRegState(OnRegStateParam &prm) {
            AccountInfo ai = getInfo();
            std::cout << (ai.regIsActive? "*** Register:" : "*** Unregister:")
                    << " code=" << prm.code << std::endl;
        }
        virtual void onIncomingCall(OnIncomingCallParam &iprm){
            std::cout << "############# Incoming Call #############" << std::endl;
            MyCall *call = new MyCall(*this, iprm.callId);
            CallOpParam prm;
            prm.statusCode = PJSIP_SC_OK;
            call->answer(prm);
        }
};


void signal_callback_handler(int signum) {
    is_shutdown = true;
}

int main()
{
    Endpoint ep;

    ep.libCreate();

    // Initialize endpoint
    EpConfig ep_cfg;
    ep.libInit( ep_cfg );

    // Create SIP transport. Error handling sample is shown
    TransportConfig tcfg;
    tcfg.port = 6060;
    try {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    } catch (Error &err) {
        std::cout << err.info() << std::endl;
        return 1;
    }

    // Start the library (worker threads etc)
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    // Configure an AccountConfig
    AccountConfig acfg;
    acfg.idUri = "sip:1000@192.168.228.130:5060";
    acfg.regConfig.registrarUri = "sip:192.168.228.130:5060";

    // Create the account
    MyAccount *acc = new MyAccount;
    acc->create(acfg);

    signal(SIGINT, signal_callback_handler);
    while(!is_shutdown){
       sleep(1);
    }

    // Delete the account. This will unregister from server
    delete acc;

    return 0;
}