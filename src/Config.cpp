#include "Config.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>

void Config::loadJsonConfig() {
  std::ifstream configFile(Config::configFilePath);
  if (!configFile.is_open()) {
    std::cout << "Warn: Cannot open configuration file: "
              << Config::configFilePath << std::endl;
  }

  try {
    nlohmann::json config;
    configFile >> config;

    Config::setVariableFromConfigFile(
        config, "control_plane_ip", Config::controlPlaneIP);
    Config::setVariableFromConfigFile(
        config, "user_plane_ip", Config::UserPlaneIP);
    Config::setVariableFromConfigFile(config, "user", Config::user);
    Config::setVariableFromConfigFile(
        config, "metadata_path", Config::metadataPath);
    Config::setVariableFromConfigFile(config, "sound_path", Config::soundPath);

    if (config.contains("siprec_mode")) {
      Config::siprecMode =
          Config::getSiprecOption(config["siprec_mode"].get<std::string>());
    }
    if (config.contains("log_level")) {
      Config::logLevel =
          Config::getLogLevelOption(config["log_level"].get<std::string>());
    }
  } catch (nlohmann::json::parse_error& e) {
    std::cout << "Error: Failed to parse JSON file. " << e.what() << std::endl;
  }
}

void Config::setVariableFromConfigFile(const nlohmann::json& config,
    const std::string& key, std::string& variable) {
  if (config.contains(key)) {
    variable = config[key].get<std::string>();
  }
}

pjsua_sip_siprec_use Config::getSiprecOption(const std::string& option) {
  if (option == "inactive") {
    return PJSUA_SIP_SIPREC_INACTIVE;
  }
  if (option == "optional") {
    return PJSUA_SIP_SIPREC_OPTIONAL;
  }
  if (option == "mandatory") {
    return PJSUA_SIP_SIPREC_MANDATORY;
  }

  throw std::invalid_argument("Invalid siprec_mode value in configuration.");
}

LogLevel Config::getLogLevelOption(const std::string& option) {
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

  throw std::invalid_argument("Invalid log_level value in configuration.");
}

// Sets the default variables
std::string Config::configFilePath = "/etc/srs/config.json";
std::string Config::controlPlaneIP = "";
std::string Config::userPlaneIP = "";
std::string Config::listenPort = "5060";
std::string Config::user = "srs";
std::string Config::metadataPath = "/var/srs/metadata";
std::string Config::soundPath = "/var/srs/sound";
LogLevel Config::logLevel = LogLevel::DEBUG;
pjsua_sip_siprec_use Config::siprecMode = PJSUA_SIP_SIPREC_OPTIONAL;