#include "SRSAccount.hpp"
#include "Config.hpp"

#include <iostream>
#include <csignal>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>

using pj::AccountConfig;
using pj::Endpoint;
using pj::EpConfig;
using pj::Error;
using pj::TransportConfig;
using std::cout;
using std::endl;
using std::make_unique;
using std::unique_ptr;

// Flag to indicate whether a shutdown signal has been received
bool isShutdown = false;

/* Signal handler function that sets the isShutdown flag when a signal is received */
void signalCallbackHandler(int /*signum*/) {
  isShutdown = true;
}

int main() {
  Config::LoadJsonConfig();

  Endpoint endpoint;
  endpoint.libCreate();

  EpConfig endpointConfig;
  endpointConfig.logConfig.level = Config::log_level;
  endpoint.libInit(endpointConfig);
  // Disable audio input/output with a null device
  endpoint.audDevManager().setNullDev();

  TransportConfig transportConfig;
  if (!Config::control_plane_ip.empty()) {
    transportConfig.boundAddress = Config::control_plane_ip;
  }
  transportConfig.port = stoi(Config::listen_port);
  try {
    endpoint.transportCreate(PJSIP_TRANSPORT_UDP, transportConfig);
  } catch (Error& err) {
    cout << err.info() << endl;
    return 1;
  }

  endpoint.libStart();
  cout << "*** PJSUA2 STARTED ***" << endl;

  AccountConfig accountConfig;
  accountConfig.idUri = "sip:" + Config::user + "@" + "service";
  accountConfig.regConfig.registrarUri = "";
  accountConfig.sipConfig.authCreds.clear();
  accountConfig.callConfig.siprecUse = Config::siprec_mode;
  if (!Config::user_plane_ip.empty()) {
    accountConfig.mediaConfig.transportConfig.boundAddress =
        Config::user_plane_ip;
  }

  // Create the account
  unique_ptr<SRSAccount> account = make_unique<SRSAccount>();
  account->create(accountConfig);

  // Register the signal handler for SIGINT (Ctrl+C)
  signal(SIGINT, signalCallbackHandler);

  // Main loop that keeps the application running until a shutdown signal is received
  while (!isShutdown) {
    sleep(1);
  }

  return 0;
}
