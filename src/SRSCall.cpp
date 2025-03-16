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

SRSCall::~SRSCall() {}

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
  CallInfo call_info = getInfo();
  int recorder = media_index + 1;
  return call_info.callIdString + "-" + to_string(recorder) + ".wav";
}

string SRSCall::GetMetadataFileName() {
  CallInfo call_info = getInfo();
  return call_info.callIdString + "-" + "Metadata" + ".txt";
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
  int call_id = getId();
  pjsua_call* call = &pjsua_var.calls[call_id];
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
  CallInfo call_info = getInfo();
  if (call_info.state == PJSIP_INV_STATE_CONNECTING ||
      call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
    PrintCallState(call_info.stateText, call_info.localUri, call_info.remoteUri,
        call_info.connectDuration.sec, call_info.callIdString);
    if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {
      delete this;
    }
  }
}

void SRSCall::onCallMediaState(OnCallMediaStateParam& /*params*/) {
  CallInfo call_info = getInfo();
  CreateDirectory(Config::sound_path);
  CreateDirectory(Config::metadata_path);
  for (int media_index = 0; media_index < call_info.media.size(); media_index++) {
    if (call_info.media[media_index].type == PJMEDIA_TYPE_AUDIO) {
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
