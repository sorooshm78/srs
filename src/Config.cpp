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
            std::cout << "Warn: Cannot open configuration file: " << Config::configFilePath << std::endl;
        }

        try {
            nlohmann::json config;
            configFile >> config;

            Config::setConfigValue(config, "listen_ip", Config::listenIP);  
            Config::setConfigValue(config, "listen_port", Config::listenPort);  
            Config::setConfigValue(config, "user", Config::user);
            Config::setConfigValue(config, "metadata_path", Config::metadataPath);
            Config::setConfigValue(config, "sound_path", Config::soundPath);

            if (config.contains("siprec_mode")) {
                Config::siprecMode = Config::getSiprecOption(config["siprec_mode"].get<std::string>());
            }
        } catch (nlohmann::json::parse_error& e) {
            std::cout << "Error: Failed to parse JSON file. " << e.what() << std::endl;
        }
    }


void Config::setConfigValue(const nlohmann::json& config, const std::string& key, std::string& variable) {
    if (config.contains(key)) {
        variable = config[key].get<std::string>();
    }
}


pjsua_sip_siprec_use Config::getSiprecOption(const std::string& option) {
    if (option == "inactive") {
        return PJSUA_SIP_SIPREC_INACTIVE;
    } else if (option == "optional") {
        return PJSUA_SIP_SIPREC_OPTIONAL;
    } else if (option == "mandatory") {
        return PJSUA_SIP_SIPREC_MANDATORY;
    } else {
        throw std::invalid_argument("Invalid siprec_mode value in configuration.");
    }
}


// Sets the default variables
std::string Config::configFilePath = "/etc/srs/config.json";
std::string Config::listenIP = "192.168.21.88";
std::string Config::listenPort = "5060";
std::string Config::user = "1010";
std::string Config::metadataPath = "/var/srs/metadata";
std::string Config::soundPath = "/var/srs/sound";
pjsua_sip_siprec_use Config::siprecMode = PJSUA_SIP_SIPREC_OPTIONAL;