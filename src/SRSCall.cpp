#include "SRSCall.hpp"
#include "Config.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <gsl/gsl>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>

SRSCall::SRSCall(pj::Account& account, int callID)
    : pj::Call(account, callID) {}

void SRSCall::printCallState(const std::string& state,
    const std::string& localUri, const std::string& remoteUri,
    int64_t connectDuration, const std::string& callID) {
  std::cout << "########## "
            << "Call-ID:" << callID;
  std::cout << "\t";
  std::cout << "State:" << state;
  std::cout << "\t";
  std::cout << "(" << remoteUri << " -> " << localUri << ")";
  std::cout << "\t";
  std::cout << "Duration:" << connectDuration << "sec";
  std::cout << std::endl;
}

std::string SRSCall::getWavFileName(int mediaIndex) {
  pj::CallInfo callInfo = getInfo();
  int recorder = mediaIndex + 1;
  return callInfo.callIdString + "-" + std::to_string(recorder) + ".wav";
}

std::string SRSCall::getMetadataFileName() {
  pj::CallInfo callInfo = getInfo();
  return callInfo.callIdString + "-" + "Metadata" + ".txt";
}

std::string SRSCall::getFullPath(
    std::string path, const std::string& fileName) {
  Expects(!path.empty());
  Expects(!fileName.empty());

  if (path.back() != '/' && path.back() != '\\') {
    path += '/';
  }
  return path + fileName;
}

void SRSCall::saveAudioMedia(const pj::AudioMedia& audioMedia, int mediaIndex) {
  Expects((mediaIndex == 0) || (mediaIndex == 1));
  std::string path = getFullPath(Config::soundPath, getWavFileName(mediaIndex));
  if (mediaIndex == 0) {
    recorder1.createRecorder(path);
    audioMedia.startTransmit(recorder1);
  }
  if (mediaIndex == 1) {
    recorder2.createRecorder(path);
    audioMedia.startTransmit(recorder2);
  }
}

void SRSCall::saveMetadata() {
  int callId = getId();
  pjsua_call* call = &pjsua_var.calls[callId];
  Expects(call != nullptr);
  std::string metadata =
      std::string(call->siprec_metadata.ptr, call->siprec_metadata.slen);
  std::string path = getFullPath(Config::metadataPath, getMetadataFileName());
  std::ofstream file(path);

  Expects(file.is_open());
  file << metadata;
  file.close();
}

void SRSCall::onCallState(pj::OnCallStateParam& /*param*/) {
  pj::CallInfo callInfo = getInfo();
  if (callInfo.state == PJSIP_INV_STATE_CONNECTING ||
      callInfo.state == PJSIP_INV_STATE_DISCONNECTED) {
    printCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri,
        callInfo.connectDuration.sec, callInfo.callIdString);
  }
}

void SRSCall::onCallMediaState(pj::OnCallMediaStateParam& /*params*/) {
  pj::CallInfo callInfo = getInfo();
  createDirectory(Config::soundPath);
  createDirectory(Config::metadataPath);
  for (int mediaIndex = 0; mediaIndex < callInfo.media.size(); mediaIndex++) {
    if (callInfo.media[mediaIndex].type == PJMEDIA_TYPE_AUDIO) {
      pj::AudioMedia audioMedia = getAudioMedia(mediaIndex);
      saveAudioMedia(audioMedia, mediaIndex);
    }
  }
  saveMetadata();
}

void SRSCall::createDirectory(const std::string& path) {
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directories(path);
  }
  Ensures(std::filesystem::exists(path));
}
