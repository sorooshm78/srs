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
bool is_shutdown = false;

/* Signal handler function that sets the is_shutdown flag when a signal is received */
void SignalCallbackHandler(int /*signum*/) {
  is_shutdown = true;
}

int main() {
  Config::LoadJsonConfig();

  Endpoint endpoint;
  endpoint.libCreate();

  EpConfig endpoint_config;
  endpoint_config.logConfig.level = static_cast<int>(Config::log_level);;
  endpoint.libInit(endpoint_config);
  // Disable audio input/output with a null device
  endpoint.audDevManager().setNullDev();

  TransportConfig transport_config;
  if (!Config::control_plane_ip.empty()) {
    transport_config.boundAddress = Config::control_plane_ip;
  }
  transport_config.port = stoi(Config::listen_port);
  try {
    endpoint.transportCreate(PJSIP_TRANSPORT_UDP, transport_config);
  } catch (Error& err) {
    cout << err.info() << endl;
    return 1;
  }

  endpoint.libStart();
  cout << "*** PJSUA2 STARTED ***" << endl;

  AccountConfig account_config;
  account_config.idUri = "sip:" + Config::user + "@" + "service";
  account_config.regConfig.registrarUri = "";
  account_config.sipConfig.authCreds.clear();
  account_config.callConfig.siprecUse = Config::siprec_mode;
  if (!Config::user_plane_ip.empty()) {
    account_config.mediaConfig.transportConfig.boundAddress =
        Config::user_plane_ip;
  }

  // Create the account
  unique_ptr<SRSAccount> account = make_unique<SRSAccount>();
  account->create(account_config);

  // Register the signal handler for SIGINT (Ctrl+C)
  signal(SIGINT, SignalCallbackHandler);

  // Main loop that keeps the application running until a shutdown signal is received
  while (!is_shutdown) {
    sleep(1);
  }

  return 0;
}
