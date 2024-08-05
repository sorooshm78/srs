#include <pjsua2.hpp>
#include <iostream>

using namespace pj;

// Subclass to extend the Account and get notifications etc.
class MyAccount : public Account {
    public:
        virtual void onRegState(OnRegStateParam &prm) {
            AccountInfo ai = getInfo();
            std::cout << (ai.regIsActive? "*** Register:" : "*** Unregister:")
                    << " code=" << prm.code << std::endl;
        }
        // virtual void onIncomingCall(OnIncomingCallParam &iprm){
        //     std::cout << "############# Incoming Call #############" << std::endl;
        //     Call *call = new MyCall(*this, iprm.callId);
        //     CallOpParam prm;
        //     prm.statusCode = PJSIP_SC_OK;
        //     call->answer(prm);
        // }
};

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

    // Here we don't have anything else to do..
    pj_thread_sleep(10 * 1000); // 10 second sleep

    // Delete the account. This will unregister from server
    delete acc;

    // This will implicitly shutdown the library
    return 0;
}