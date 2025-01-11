#include "SRSAccount.hpp"
#include "Config.hpp"
#include <pjsua2.hpp>
#include <iostream>
#include <csignal>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <pjsua-lib/pjsua_internal.h>

using namespace pj;
using namespace std;


bool isShutdown = false;

void signalCallbackHandler(int signum)
{
    isShutdown = true;
}

int main(int argc, char* argv[])
{
    Config::loadJsonConfig();

    Endpoint endpoint;
    endpoint.libCreate();

    EpConfig endpointConfig;
    endpointConfig.logConfig.level = Config::logLevel;
    endpoint.libInit(endpointConfig);
    endpoint.audDevManager().setNullDev();

    TransportConfig transportConfig;
    transportConfig.port = stoi(Config::listenPort);
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
    accountConfig.idUri = "sip:" + Config::user + "@" + Config::listenIP;
    accountConfig.regConfig.registrarUri = "";
    accountConfig.sipConfig.authCreds.clear();
    accountConfig.callConfig.siprecUse = Config::siprecMode;

    // Create the account
    unique_ptr<SRSAccount> account = make_unique<SRSAccount>();
    account->create(accountConfig);

    // Register the signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, signalCallbackHandler);

    // Main loop that keeps the application running until a shutdown signal is received
    while (!isShutdown)
    {
        sleep(1);
    }

    return 0;
}
