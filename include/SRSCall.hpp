#include <pjsua2.hpp>
#include <string>
#include <cstdint>
#include <iostream>


class SRSCall : public pj::Call
{
public:
    /* Constructor */
    explicit SRSCall(pj::Account& account, int callID = PJSUA_INVALID_ID);

    /* Prints the current state of a call */
    void printCallState(const std::string& state, const std::string& localUri, const std::string& remoteUri, int64_t connectDuration, const std::string& callID);

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
     * mediaIndex can only be 0 or 1
     */
    auto getWavFileName(int mediaIndex) -> std::string;

    /* Generates a metadata file name based on the call ID. */
    auto getMetadataFileName() -> std::string;

    /* Constructs a full file path by combining a directory path and a filename. */
    auto getFullPath(std::string path, const std::string& fileName) -> std::string;
    
    /* Creates a directory at the specified path if it does not already exist. */
    void createDirectory(const std::string& path);
    
    /* Store the call audio in a ".wav" file. */
    void saveAudioMedia(const pj::AudioMedia& audioMedia, int mediaIndex);
    
    /* Store the SIPREC call metadata. */
    void saveMetadata();
};