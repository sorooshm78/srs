#include <pjsua2.hpp>
#include <iostream>
#include <csignal>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#include <pjsua-lib/pjsua_internal.h>

using json = nlohmann::json;
using namespace pj;
using namespace std;

bool isShutdown = false;
string configFilePath = "/etc/srs/config.json";


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


void createDirectory(string path)
{
    if (!filesystem::exists(path)) {
        filesystem::create_directories(path);
    }
}


class Config {
public:
    static int listenPort;
    static string user;
    static string metadataPath;
    static string soundPath;
    static pjsua_sip_siprec_use siprecUse;


    static void loadConfig() {
        ifstream configFile(configFilePath);
        if (!configFile.is_open()) {
            cout << "Warn: Cannot open configuration file: " << configFilePath << endl;
        }

        try {
            json config;
            configFile >> config;

            setConfigValue(config, "listen_port", listenPort);
            setConfigValue(config, "user", user);
            setConfigValue(config, "metadata_path", metadataPath);
            setConfigValue(config, "sound_path", soundPath);

            if (config.contains("siprec_use")) {
                siprecUse = getSiprecOption(config["siprec_use"].get<string>());
            }
        } catch (json::parse_error& e) {
            cout << "Error: Failed to parse JSON file. " << e.what() << endl;
        }
    }

private:
    template <typename T>
    static void setConfigValue(const json& config, const string& key, T& variable) {
        if (config.contains(key)) {
            variable = config[key].get<T>();
        }
    }

    static pjsua_sip_siprec_use getSiprecOption(const string& option) {
        if (option == "inactive") {
            return PJSUA_SIP_SIPREC_INACTIVE;
        } else if (option == "optional") {
            return PJSUA_SIP_SIPREC_OPTIONAL;
        } else if (option == "mandatory") {
            return PJSUA_SIP_SIPREC_MANDATORY;
        } else {
            throw invalid_argument("Invalid siprec_use value in configuration.");
        }
    }
};


int Config::listenPort = 5060;
string Config::user = "1010";
string Config::metadataPath = "/var/srs/metadata";
string Config::soundPath = "/var/srs/sound";
pjsua_sip_siprec_use Config::siprecUse = PJSUA_SIP_SIPREC_OPTIONAL;


class MyCall : public Call
{
public:
    AudioMediaRecorder recorder1;
    AudioMediaRecorder recorder2;

    MyCall(Account& account, int callID = PJSUA_INVALID_ID)
        : Call(account, callID)
    {
    }

    void printCallState(string state, string localUri, string remoteUri, long connectDuration, string callID)
    {
        cout << "########## " << "Call-ID:" << callID;
        cout << "\t";
        cout << "State:" << state;
        cout << "\t";
        cout << "(" << remoteUri << " -> " << localUri << ")";
        cout << "\t";
        cout << "Duration:" << connectDuration << "sec";
        cout << endl;
    }

    string getWavFileName(int media_index)
    {
        CallInfo callInfo = getInfo();
        int recorder = media_index + 1;
        return callInfo.callIdString + "-" + to_string(recorder) +  ".wav";
    }

    string getMetadataFileName()
    {
        CallInfo callInfo = getInfo();
        return callInfo.callIdString + "-" + "Metadata" +  ".txt";
    }

    string getFullPath(string path, string filename)
    {
        if (path.back() != '/' && path.back() != '\\') {
            path += '/';
        }
        return path + filename;
    }

    void saveAudioMedia(AudioMedia audioMedio, int media_index)
    {
        string path = getFullPath(Config::soundPath, getWavFileName(media_index));
        if(media_index == 0){
            recorder1.createRecorder(path);
            audioMedio.startTransmit(recorder1);
        }
        if(media_index == 1){
            recorder2.createRecorder(path);
            audioMedio.startTransmit(recorder2);
        }
    }

    void saveMetadata()
    {
        int id = getId();
        pjsua_call *call = &pjsua_var.calls[id];
        string metadata = string(call->siprec_metadata.ptr, call->siprec_metadata.slen);
        
        string path = getFullPath(Config::metadataPath, getMetadataFileName());
        ofstream file(path);
        
        if (file.is_open()) {
            file << metadata;
            file.close();
        } else {
            cout << "Unable to open file for writing!" << endl;
        }
    }

    virtual void onCallState(OnCallStateParam& param)
    {
        CallInfo callInfo = getInfo();
        if (callInfo.state == PJSIP_INV_STATE_CONNECTING || callInfo.state == PJSIP_INV_STATE_DISCONNECTED)
        {
            printCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri, callInfo.connectDuration.sec,
                           callInfo.callIdString);
        }
    }

    virtual void onCallMediaState(OnCallMediaStateParam& params)
    {
        CallInfo callInfo = getInfo();
        createDirectory(Config::soundPath);
        createDirectory(Config::metadataPath);
        for (unsigned media_index = 0; media_index < callInfo.media.size(); media_index++)
        {
            if (callInfo.media[media_index].type == PJMEDIA_TYPE_AUDIO)
            {
                AudioMedia audioMedia = getAudioMedia(media_index);
                saveAudioMedia(audioMedia, media_index);
            }
        }
        saveMetadata();
    }
};


class MyAccount : public Account
{
public:
    void onRegState(OnRegStateParam& param) override
    {
        AccountInfo accountInfo = getInfo();
        cout << (accountInfo.regIsActive ? "*** Register:" : "*** Unregister:")
            << " code=" << param.code << endl;
    }

    void onIncomingCall(OnIncomingCallParam& incomingParam) override
    {
        MyCall* call = new MyCall(*this, incomingParam.callId);
        CallOpParam param;
        param.statusCode = PJSIP_SC_OK;
        param.opt.audioCount = 2;
        param.opt.videoCount = 0;
        call->answer(param);
    }
};


int main(int argc, char* argv[])
{
    Config::loadConfig();

    Endpoint endpoint;
    endpoint.libCreate();

    EpConfig endpointConfig;
    endpointConfig.logConfig.level = loglevel::WARNING;
    endpointConfig.logConfig.consoleLevel = loglevel::WARNING;
    endpoint.libInit(endpointConfig);
    endpoint.audDevManager().setNullDev();

    TransportConfig transportConfig;
    transportConfig.port = Config::listenPort;
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
    accountConfig.idUri = "sip:" + Config::user + "@192.168.21.88";
    accountConfig.regConfig.registrarUri = "";
    accountConfig.sipConfig.authCreds.clear();
    accountConfig.callConfig.siprecUse = Config::siprecUse;

    // Create the account
    auto* account = new MyAccount;
    account->create(accountConfig);

    signal(SIGINT, signalCallbackHandler);
    while (!isShutdown)
    {
        sleep(1);
    }

    delete account;

    return 0;
}
