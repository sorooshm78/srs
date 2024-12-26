#include <pjsua2.hpp>
#include <string>
#include <iostream>


class SRSCall : public pj::Call
{
public:

    SRSCall(pj::Account& account, int callID = PJSUA_INVALID_ID);
    void printCallState(std::string state, std::string localUri, std::string remoteUri, long connectDuration, std::string callID);
    virtual void onCallMediaState(pj::OnCallMediaStateParam& params);
    virtual void onCallState(pj::OnCallStateParam& param);

private:
    pj::AudioMediaRecorder recorder1;
    pj::AudioMediaRecorder recorder2;

    std::string getWavFileName(int media_index);
    std::string getMetadataFileName();
    std::string getFullPath(std::string path, std::string filename);
    void createDirectory(std::string path);
    void saveAudioMedia(pj::AudioMedia audioMedio, int media_index);
    void saveMetadata();
};