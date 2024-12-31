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
    endpointConfig.logConfig.level = loglevel::WARNING;
    endpointConfig.logConfig.consoleLevel = loglevel::WARNING;
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
    SRSAccount* account = new SRSAccount;
    account->create(accountConfig);

    signal(SIGINT, signalCallbackHandler);
    while (!isShutdown)
    {
        sleep(1);
    }

    delete account;

    return 0;
}
