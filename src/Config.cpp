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

void Config::LoadJsonConfig() {
  ifstream config_file(Config::config_file_path);
  if (!config_file.is_open()) {
    cout << "Warn: Cannot open configuration file: " << Config::config_file_path
         << endl;
  }

  try {
    json config;
    config_file >> config;

    Config::SetVariableFromConfigFile(
        config, "control_plane_ip", Config::control_plane_ip);
    Config::SetVariableFromConfigFile(
        config, "user_plane_ip", Config::user_plane_ip);
    Config::SetVariableFromConfigFile(config, "user", Config::user);
    Config::SetVariableFromConfigFile(
        config, "metadata_path", Config::metadata_path);
    Config::SetVariableFromConfigFile(config, "sound_path", Config::sound_path);

    if (config.contains("siprec_mode")) {
      Config::siprec_mode =
          Config::GetSiprecOption(config["siprec_mode"].get<string>());
    }
    if (config.contains("log_level")) {
      Config::log_level =
          Config::GetLogLevelOption(config["log_level"].get<string>());
    }
  } catch (parse_error& e) {
    cout << "Error: Failed to parse JSON file. " << e.what() << endl;
  }
}

void Config::SetVariableFromConfigFile(
    const nlohmann::json& config, const string& key, string& variable) {
  if (config.contains(key)) {
    variable = config[key].get<string>();
  }
}

pjsua_sip_siprec_use Config::GetSiprecOption(const string& option) {
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

LogLevel Config::GetLogLevelOption(const string& option) {
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
string Config::config_file_path = "/etc/srs/config.json";
string Config::control_plane_ip = "";
string Config::user_plane_ip = "";
string Config::listen_port = "5060";
string Config::user = "srs";
string Config::metadata_path = "/var/srs/metadata";
string Config::sound_path = "/var/srs/sound";
LogLevel Config::log_level = LogLevel::DEBUG;
pjsua_sip_siprec_use Config::siprec_mode = PJSUA_SIP_SIPREC_OPTIONAL;