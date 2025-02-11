#include "Config.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>

using nlohmann::json;
using nlohmann::detail::parse_error;
using std::cout;
using std::endl;
using std::ifstream;
using std::invalid_argument;
using std::string;

void Config::loadJsonConfig() {
  ifstream configFile(Config::configFilePath);
  if (!configFile.is_open()) {
    cout << "Warn: Cannot open configuration file: " << Config::configFilePath
         << endl;
  }

  try {
    json config;
    configFile >> config;

    Config::setVariableFromConfigFile(
        config, "control_plane_ip", Config::controlPlaneIP);
    Config::setVariableFromConfigFile(
        config, "user_plane_ip", Config::userPlaneIP);
    Config::setVariableFromConfigFile(config, "user", Config::user);
    Config::setVariableFromConfigFile(
        config, "metadata_path", Config::metadataPath);
    Config::setVariableFromConfigFile(config, "sound_path", Config::soundPath);

    if (config.contains("siprec_mode")) {
      Config::siprecMode =
          Config::getSiprecOption(config["siprec_mode"].get<string>());
    }
    if (config.contains("log_level")) {
      Config::logLevel =
          Config::getLogLevelOption(config["log_level"].get<string>());
    }
  } catch (parse_error& e) {
    cout << "Error: Failed to parse JSON file. " << e.what() << endl;
  }
}

void Config::setVariableFromConfigFile(
    const nlohmann::json& config, const string& key, string& variable) {
  if (config.contains(key)) {
    variable = config[key].get<string>();
  }
}

pjsua_sip_siprec_use Config::getSiprecOption(const string& option) {
  if (option == "inactive") {
    return PJSUA_SIP_SIPREC_INACTIVE;
  }
  if (option == "optional") {
    return PJSUA_SIP_SIPREC_OPTIONAL;
  }
  if (option == "mandatory") {
    return PJSUA_SIP_SIPREC_MANDATORY;
  }

  throw invalid_argument("Invalid siprec_mode value in configuration.");
}

LogLevel Config::getLogLevelOption(const string& option) {
  if (option == "fatal_error") {
    return LogLevel::FATAL_ERROR;
  }
  if (option == "error") {
    return LogLevel::ERROR;
  }
  if (option == "warning") {
    return LogLevel::WARNING;
  }
  if (option == "info") {
    return LogLevel::INFO;
  }
  if (option == "debug") {
    return LogLevel::DEBUG;
  }
  if (option == "trace") {
    return LogLevel::TRACE;
  }
  if (option == "detailed_trace") {
    return LogLevel::DETAILED_TRACE;
  }

  throw invalid_argument("Invalid log_level value in configuration.");
}

// Sets the default variables
string Config::configFilePath = "/etc/srs/config.json";
string Config::controlPlaneIP = "";
string Config::userPlaneIP = "";
string Config::listenPort = "5060";
string Config::user = "srs";
string Config::metadataPath = "/var/srs/metadata";
string Config::soundPath = "/var/srs/sound";
LogLevel Config::logLevel = LogLevel::DEBUG;
pjsua_sip_siprec_use Config::siprecMode = PJSUA_SIP_SIPREC_OPTIONAL;