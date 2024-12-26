#include "SRSCall.hpp"
#include "Config.hpp"

#include <pjsua2.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem> 

#include <pjsua-lib/pjsua_internal.h>


using namespace std;
using namespace pj;

SRSCall::SRSCall(Account& account, int callID)
    : pj::Call(account, callID)
{
}

void SRSCall::printCallState(string state, string localUri, string remoteUri, long connectDuration, string callID)
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

string SRSCall::getWavFileName(int media_index)
{
    CallInfo callInfo = getInfo();
    int recorder = media_index + 1;
    return callInfo.callIdString + "-" + to_string(recorder) +  ".wav";
}

string SRSCall::getMetadataFileName()
{
    CallInfo callInfo = getInfo();
    return callInfo.callIdString + "-" + "Metadata" +  ".txt";
}

string SRSCall::getFullPath(string path, string filename)
{
    if (path.back() != '/' && path.back() != '\\') {
        path += '/';
    }
    return path + filename;
}

void SRSCall::saveAudioMedia(AudioMedia audioMedio, int media_index)
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

void SRSCall::saveMetadata()
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

void SRSCall::onCallState(OnCallStateParam& param)
{
    CallInfo callInfo = getInfo();
    if (callInfo.state == PJSIP_INV_STATE_CONNECTING || callInfo.state == PJSIP_INV_STATE_DISCONNECTED)
    {
        printCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri, callInfo.connectDuration.sec,
                        callInfo.callIdString);
    }
}

void SRSCall::onCallMediaState(OnCallMediaStateParam& params)
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

void SRSCall::createDirectory(string path)
{
    if (!filesystem::exists(path)) {
        filesystem::create_directories(path);
    }
}
