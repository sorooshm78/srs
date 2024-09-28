// #include <pjsua-lib/pjsua.h>
// #include <pjsip.h>
// #include <iostream>


// #define SIP_PORT 5060
// #define LOG_LEVEL 3


// void print_sdp_info(const pjmedia_sdp_session *sdp_session) {
//     std::cout << "Session name: " << std::string(sdp_session->name.ptr, sdp_session->name.slen) << std::endl;
//     if (sdp_session->conn) {
//         std::cout << "Connection Address: " << std::string(sdp_session->conn->addr.ptr, sdp_session->conn->addr.slen)  << std::endl;
//     }
//     for (unsigned i = 0; i < sdp_session->media_count; ++i) {
//         pjmedia_sdp_media *media = sdp_session->media[i];

//         std::cout << "Media Type: " 
//                 << std::string(media->desc.media.ptr, media->desc.media.slen) 
//                 << std::endl;

//         std::cout << "Port: " << media->desc.port << std::endl;

//         std::cout << "Transport: " 
//                 << std::string(media->desc.transport.ptr, media->desc.transport.slen) 
//                 << std::endl;

//         for (unsigned j = 0; j < media->desc.fmt_count; ++j) {
//             std::cout << "Format: " 
//                     << std::string(media->desc.fmt[j].ptr, media->desc.fmt[j].slen) 
//                     << std::endl;
//         }
//     }
// }


// static pj_bool_t on_rx_request(pjsip_rx_data *rdata)
// {
//     pjsip_msg *msg = rdata->msg_info.msg;
//     if (pjsip_method_cmp(&msg->line.req.method, &pjsip_invite_method) != 0) {
//         return PJ_TRUE;
//     }

//     pjmedia_sdp_session *sdp_session;

//     if (msg->body == NULL || msg->body->data == NULL) {
//         std::cout << "Message body is NULL." << std::endl;
//         return PJ_FALSE;
//     }

//     pj_pool_t *pool = pjsua_pool_create("sdp_parse_pool", 1024, 1024);
//     if (!pool) {
//         std::cout << "Failed to create memory pool." << std::endl;
//         return PJ_FALSE;
//     }

//     pj_status_t status = pjmedia_sdp_parse(pool, (char*)msg->body->data, msg->body->len, &sdp_session);

//     if (status == PJ_SUCCESS) {
//         std::cout << "########################### SDP parsed successfully." << std::endl;
//     } else {
//         std::cout << "###################### Failed to parse SDP." << std::endl;
//     }

//     print_sdp_info(sdp_session);
//     pj_pool_release(pool);
//     return PJ_TRUE;
// }


// static pjsip_module srs_module ={
//     nullptr, nullptr,                               // prev, next
//     { const_cast<char*>("srs-module"), 10 } ,       // Name
//     -1,                                             // ID
//     PJSIP_MOD_PRIORITY_TRANSPORT_LAYER,             // Priority
//     nullptr,                                        // load()
//     nullptr,                                        // start()
//     nullptr,                                        // stop()
//     nullptr,                                        // unload()
//     &on_rx_request,                                 // on_rx_request()
//     nullptr,                                        // on_rx_response()
//     nullptr,                                        // on_tx_request()
//     nullptr,                                        // on_tx_response()
//     nullptr,                                        // on_tsx_state()
// };


// // Initialization of PJSIP
// void init_pjsip()
// {
//     pjsua_config cfg;
//     pjsua_logging_config log_cfg;

//     // Initialize PJSUA (PJSIP's high level API)
//     pjsua_create();

//     // Configure PJSUA
//     pjsua_config_default(&cfg);

//     pjsua_logging_config_default(&log_cfg);
//     log_cfg.console_level = LOG_LEVEL;

//     // Initialize PJSUA with configuration
//     pjsua_init(&cfg, &log_cfg, nullptr);

//     // Add UDP transport on the specified port (5060 by default)
//     pjsua_transport_config transport_cfg;
//     pjsua_transport_config_default(&transport_cfg);
//     transport_cfg.port = SIP_PORT;
//     pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, nullptr);

//     // Start PJSUA
//     pjsua_start();

//     // Register custom module to intercept incoming SIP messages
//     pjsip_endpt_register_module(pjsua_get_pjsip_endpt(), &srs_module);
// }


// int main()
// {
//     try {
//         // Initialize PJSIP library
//         init_pjsip();

//         std::cout << "SIP server is listening on port " << SIP_PORT << std::endl;

//         // Keep the server running indefinitely
//         while (true) {
//             // Perform necessary SIP operations
//             pjsua_handle_events(100);
//         }

//         // Clean up PJSIP before exit
//         pjsua_destroy();
//     } catch (const std::exception &e) {
//         std::cerr << "Exception occurred: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;

// }


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
    AudioMediaRecorder recorder;

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
        for (unsigned i = 0; i < callInfo.media.size(); i++)
        {
            if (callInfo.media[i].type == PJMEDIA_TYPE_AUDIO)
            {
                AudioMedia audioMedia = getAudioMedia(i);
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
    endpointConfig.logConfig.level = loglevel::DEBUG;
    endpointConfig.logConfig.consoleLevel = loglevel::DEBUG;
    endpoint.libInit(endpointConfig);


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
