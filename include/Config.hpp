#include <iostream>
#include <nlohmann/json.hpp>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>


enum LogLevel
{
    FATAL_ERROR = 0,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE,
    DETAILED_TRACE,
};

class Config {
public:
    static std::string configFilePath;
    static std::string listenIP;
    static std::string listenPort;
    static std::string user;
    static std::string metadataPath;
    static std::string soundPath;
    static LogLevel logLevel;
    static pjsua_sip_siprec_use siprecMode;

    /**
     * This function reads the configurations from the file
     * and sets them into the variables.
     */
    static void loadJsonConfig();

    /* Converts the config string value to pjsua_sip_siprec_use. */
    static auto getSiprecOption(const std::string& option) -> pjsua_sip_siprec_use;
    
    /* Converts the config string value to LogLevel.*/
    static auto getLogLevelOption(const std::string& option) -> LogLevel;

    /* Sets the variable with 'key' name from config file */
    static void setVariableFromConfigFile(const nlohmann::json& config, const std::string& key, std::string& variable);
};
