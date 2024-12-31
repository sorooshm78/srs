#include <iostream>
#include <nlohmann/json.hpp>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>


class Config {
public:
    static std::string configFilePath;
    static std::string listenIP;
    static std::string listenPort;
    static std::string user;
    static std::string metadataPath;
    static std::string soundPath;
    static pjsua_sip_siprec_use siprecMode;

    /**
     * This function reads the configurations from the file
     * and sets them into the variables.
     */
    static void loadJsonConfig();

    /* Converts the config string value to pjsua_sip_siprec_use. */
    static pjsua_sip_siprec_use getSiprecOption(const std::string& option);
    
    /* Sets the variables */
    static void setConfigValue(const nlohmann::json& config, const std::string& key, std::string& variable);
};
