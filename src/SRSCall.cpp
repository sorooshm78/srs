#include "SRSCall.hpp"
#include "Config.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <gsl/gsl>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua_internal.h>

using pj::Account;
using pj::AudioMedia;
using pj::CallInfo;
using pj::OnCallMediaStateParam;
using pj::OnCallStateParam;
using std::cout;
using std::endl;
using std::ofstream;
using std::string;
using std::to_string;
using std::filesystem::create_directories;
using std::filesystem::exists;

SRSCall::SRSCall(Account& account, int callID) : Call(account, callID) {}

void SRSCall::printCallState(const string& state, const string& localUri,
    const string& remoteUri, int64_t connectDuration, const string& callID) {
  cout << "########## "
       << "Call-ID:" << callID;
  cout << "\t";
  cout << "State:" << state;
  cout << "\t";
  cout << "(" << remoteUri << " -> " << localUri << ")";
  cout << "\t";
  cout << "Duration:" << connectDuration << "sec";
  cout << endl;
}

string SRSCall::getWavFileName(int mediaIndex) {
  CallInfo callInfo = getInfo();
  int recorder = mediaIndex + 1;
  return callInfo.callIdString + "-" + to_string(recorder) + ".wav";
}

string SRSCall::getMetadataFileName() {
  CallInfo callInfo = getInfo();
  return callInfo.callIdString + "-" + "Metadata" + ".txt";
}

string SRSCall::getFullPath(string path, const string& fileName) {
  Expects(!path.empty());
  Expects(!fileName.empty());

  if (path.back() != '/' && path.back() != '\\') {
    path += '/';
  }
  return path + fileName;
}

void SRSCall::saveAudioMedia(const AudioMedia& audioMedia, int mediaIndex) {
  Expects((mediaIndex == 0) || (mediaIndex == 1));
  string path = getFullPath(Config::soundPath, getWavFileName(mediaIndex));
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
  string metadata =
      string(call->siprec_metadata.ptr, call->siprec_metadata.slen);
  string path = getFullPath(Config::metadataPath, getMetadataFileName());
  ofstream file(path);

  Expects(file.is_open());
  file << metadata;
  file.close();
}

void SRSCall::onCallState(OnCallStateParam& /*param*/) {
  CallInfo callInfo = getInfo();
  if (callInfo.state == PJSIP_INV_STATE_CONNECTING ||
      callInfo.state == PJSIP_INV_STATE_DISCONNECTED) {
    printCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri,
        callInfo.connectDuration.sec, callInfo.callIdString);
  }
}

void SRSCall::onCallMediaState(OnCallMediaStateParam& /*params*/) {
  CallInfo callInfo = getInfo();
  createDirectory(Config::soundPath);
  createDirectory(Config::metadataPath);
  for (int mediaIndex = 0; mediaIndex < callInfo.media.size(); mediaIndex++) {
    if (callInfo.media[mediaIndex].type == PJMEDIA_TYPE_AUDIO) {
      AudioMedia audioMedia = getAudioMedia(mediaIndex);
      saveAudioMedia(audioMedia, mediaIndex);
    }
  }
  saveMetadata();
}

void SRSCall::createDirectory(const string& path) {
  if (!exists(path)) {
    create_directories(path);
  }
  Ensures(exists(path));
}
