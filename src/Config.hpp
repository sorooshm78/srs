#include <iostream>

#include <nlohmann/json.hpp>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>

enum class LogLevel {
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
  static std::string config_file_path;
  static std::string control_plane_ip;
  static std::string user_plane_ip;
  static std::string listen_port;
  static std::string user;
  static std::string metadata_path;
  static std::string sound_path;
  static LogLevel log_level;
  static pjsua_sip_siprec_use siprec_mode;

  /**
     * This function reads the configurations from the file
     * and sets them into the variables.
     */
  static void LoadJsonConfig();

  /* Converts the config string value to pjsua_sip_siprec_use. */
  static pjsua_sip_siprec_use GetSiprecOption(const std::string& option);

  /* Converts the config string value to LogLevel.*/
  static LogLevel GetLogLevelOption(const std::string& option);

  /* Sets the variable with 'key' name from config file */
  static void SetVariableFromConfigFile(const nlohmann::json& config,
      const std::string& key, std::string& variable);
};
