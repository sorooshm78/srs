#include <cstdint>
#include <iostream>
#include <string>

#include <pjsua2.hpp>

class SRSCall : public pj::Call {
public:
  /* Constructor */
  explicit SRSCall(pj::Account& account, int call_id = PJSUA_INVALID_ID);

  /* Destructor */
  virtual ~SRSCall();

  /* Prints the current state of a call */
  void PrintCallState(const std::string& state, const std::string& local_uri,
      const std::string& remote_uri, int64_t connect_duration,
      const std::string& call_id);

  /**
     * Notify application when media state in the call has changed.
     * Normal application would need to implement this callback, e.g.
     * to connect the call's media to sound device. When ICE is used,
     * this callback will also be called to report ICE negotiation
     * failure.
     */
  void onCallMediaState(pj::OnCallMediaStateParam& params) override;

  /**
     * Notify application when call state has changed.
     * Application may then query the call info to get the
     * detail call states by calling getInfo() function.
     */
  void onCallState(pj::OnCallStateParam& param) override;

private:
  pj::AudioMediaRecorder recorder1;
  pj::AudioMediaRecorder recorder2;

  /* Generates a WAV file name based on the call ID and media index.
     * media_index can only be 0 or 1
     */
  std::string GetWavFileName(int media_index);

  /* Generates a metadata file name based on the call ID. */
  std::string GetMetadataFileName();

  /* Constructs a full file path by combining a directory path and a filename. */
  std::string GetFullPath(std::string path, const std::string& file_name);

  /* Creates a directory at the specified path if it does not already exist. */
  void CreateDirectory(const std::string& path);

  /* Store the call audio in a ".wav" file. */
  void SaveAudioMedia(const pj::AudioMedia& audio_media, int media_index);

  /* Store the SIPREC call metadata. */
  void SaveMetadata();
};