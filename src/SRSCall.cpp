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

SRSCall::SRSCall(Account& account, int call_id) : Call(account, call_id) {}

void SRSCall::PrintCallState(const string& state, const string& local_uri,
    const string& remote_uri, int64_t connect_duration, const string& call_id) {
  cout << "########## "
       << "Call-ID:" << call_id;
  cout << "\t";
  cout << "State:" << state;
  cout << "\t";
  cout << "(" << remote_uri << " -> " << local_uri << ")";
  cout << "\t";
  cout << "Duration:" << connect_duration << "sec";
  cout << endl;
}

string SRSCall::GetWavFileName(int media_index) {
  CallInfo callInfo = getInfo();
  int recorder = media_index + 1;
  return callInfo.callIdString + "-" + to_string(recorder) + ".wav";
}

string SRSCall::GetMetadataFileName() {
  CallInfo callInfo = getInfo();
  return callInfo.callIdString + "-" + "Metadata" + ".txt";
}

string SRSCall::GetFullPath(string path, const string& file_name) {
  Expects(!path.empty());
  Expects(!file_name.empty());

  if (path.back() != '/' && path.back() != '\\') {
    path += '/';
  }
  return path + file_name;
}

void SRSCall::SaveAudioMedia(const AudioMedia& audio_media, int media_index) {
  Expects((media_index == 0) || (media_index == 1));
  string path = GetFullPath(Config::sound_path, GetWavFileName(media_index));
  if (media_index == 0) {
    recorder1.createRecorder(path);
    audio_media.startTransmit(recorder1);
  }
  if (media_index == 1) {
    recorder2.createRecorder(path);
    audio_media.startTransmit(recorder2);
  }
}

void SRSCall::SaveMetadata() {
  int callId = getId();
  pjsua_call* call = &pjsua_var.calls[callId];
  Expects(call != nullptr);
  string metadata =
      string(call->siprec_metadata.ptr, call->siprec_metadata.slen);
  string path = GetFullPath(Config::metadata_path, GetMetadataFileName());
  ofstream file(path);

  Expects(file.is_open());
  file << metadata;
  file.close();
}

void SRSCall::onCallState(OnCallStateParam& /*param*/) {
  CallInfo callInfo = getInfo();
  if (callInfo.state == PJSIP_INV_STATE_CONNECTING ||
      callInfo.state == PJSIP_INV_STATE_DISCONNECTED) {
    PrintCallState(callInfo.stateText, callInfo.localUri, callInfo.remoteUri,
        callInfo.connectDuration.sec, callInfo.callIdString);
  }
}

void SRSCall::onCallMediaState(OnCallMediaStateParam& /*params*/) {
  CallInfo callInfo = getInfo();
  CreateDirectory(Config::sound_path);
  CreateDirectory(Config::metadata_path);
  for (int media_index = 0; media_index < callInfo.media.size(); media_index++) {
    if (callInfo.media[media_index].type == PJMEDIA_TYPE_AUDIO) {
      AudioMedia audio_media = getAudioMedia(media_index);
      SaveAudioMedia(audio_media, media_index);
    }
  }
  SaveMetadata();
}

void SRSCall::CreateDirectory(const string& path) {
  if (!exists(path)) {
    create_directories(path);
  }
  Ensures(exists(path));
}
