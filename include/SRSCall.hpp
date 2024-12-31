#include <pjsua2.hpp>
#include <string>
#include <iostream>


class SRSCall : public pj::Call
{
public:
    /* Constructor */
    SRSCall(pj::Account& account, int callID = PJSUA_INVALID_ID);

    /* Prints the current state of a call */
    void printCallState(std::string state, std::string localUri, std::string remoteUri, long connectDuration, std::string callID);

    /**
     * Notify application when media state in the call has changed.
     * Normal application would need to implement this callback, e.g.
     * to connect the call's media to sound device. When ICE is used,
     * this callback will also be called to report ICE negotiation
     * failure.
     */
    virtual void onCallMediaState(pj::OnCallMediaStateParam& params);
    
    /**
     * Notify application when call state has changed.
     * Application may then query the call info to get the
     * detail call states by calling getInfo() function.
     */
    virtual void onCallState(pj::OnCallStateParam& param);

private:
    pj::AudioMediaRecorder recorder1;
    pj::AudioMediaRecorder recorder2;

    /* Generates a WAV file name based on the call ID and media index. */
    std::string getWavFileName(int media_index);

    /* Generates a metadata file name based on the call ID. */
    std::string getMetadataFileName();

    /* Constructs a full file path by combining a directory path and a filename. */
    std::string getFullPath(std::string path, std::string filename);
    
    /* Creates a directory at the specified path if it does not already exist. */
    void createDirectory(std::string path);
    
    /* Store the call audio in a ".wav" file. */
    void saveAudioMedia(pj::AudioMedia audioMedio, int media_index);
    
    /* Store the SIPREC call metadata. */
    void saveMetadata();
};