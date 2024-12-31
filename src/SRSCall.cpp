#include "SRSCall.hpp"
#include "Config.hpp"
#include <pjsua2.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem> 
#include <pjsua-lib/pjsua_internal.h>


SRSCall::SRSCall(pj::Account& account, int callID)
    : pj::Call(account, callID)
{
}

void SRSCall::printCallState(std::string state, std::string localUri, std::string remoteUri, long connectDuration, std::string callID)
{
    std::cout << "########## " << "Call-ID:" << callID;
    std::cout << "\t";
    std::cout << "State:" << state;
    std::cout << "\t";
    std::cout << "(" << remoteUri << " -> " << localUri << ")";
    std::cout << "\t";
    std::cout << "Duration:" << connectDuration << "sec";
    std::cout << std::endl;
}

std::string SRSCall::getWavFileName(int media_index)
{
    pj::CallInfo callInfo = getInfo();
    int recorder = media_index + 1;
    return callInfo.callIdString + "-" + std::to_string(recorder) +  ".wav";
}

std::string SRSCall::getMetadataFileName()
{
    pj::CallInfo callInfo = getInfo();
    return callInfo.callIdString + "-" + "Metadata" +  ".txt";
}

std::string SRSCall::getFullPath(std::string path, std::string filename)
{
    if (path.back() != '/' && path.back() != '\\') {
        path += '/';
    }
    return path + filename;
}

void SRSCall::saveAudioMedia(pj::AudioMedia audioMedio, int media_index)
{
    std::string path = getFullPath(Config::soundPath, getWavFileName(media_index));
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
    std::string metadata = std::string(call->siprec_metadata.ptr, call->siprec_metadata.slen);
    
    std::string path = getFullPath(Config::metadataPath, getMetadataFileName());
    std::ofstream file(path);
    
    if (file.is_open()) {
        file << metadata;
        file.close();
    } else {
        std::cout << "Unable to open file for writing!" << std::endl;
    }
}

void SRSCall::onCallState(pj::OnCallStateParam& param)
{
    pj::CallInfo callInfo = getInfo();
    if (callInfo.state == PJSIP_INV_STATE_CONNECTING || callInfo.state == PJSIP_INV_STATE_DISCONNECTED)
    {
        printCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri, callInfo.connectDuration.sec,
                        callInfo.callIdString);
    }
}

void SRSCall::onCallMediaState(pj::OnCallMediaStateParam& params)
{
    pj::CallInfo callInfo = getInfo();
    createDirectory(Config::soundPath);
    createDirectory(Config::metadataPath);
    for (unsigned media_index = 0; media_index < callInfo.media.size(); media_index++)
    {
        if (callInfo.media[media_index].type == PJMEDIA_TYPE_AUDIO)
        {
            pj::AudioMedia audioMedia = getAudioMedia(media_index);
            saveAudioMedia(audioMedia, media_index);
        }
    }
    saveMetadata();
}

void SRSCall::createDirectory(std::string path)
{
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}
