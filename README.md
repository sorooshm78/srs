# Session Recording Server
The Session Recording Server (SRS) is a robust and flexible solution designed to capture and manage session data, particularly for SIP (Session Initiation Protocol) communications. Whether you're operating a VoIP service, a call center, or any application that relies on SIP-based communications, the SRS provides essential tools to record, store, and manage both the metadata and audio streams of your sessions.

## Config 
You can modify the SRS settings.
To change the settings, create or edit the file located at `/etc/srs/config.json`
```json
{
    "listen_ip": "192.168.20.50",
    "listen_port": "5060",
    "metadata_path": "/var/srs/metadata",
    "sound_path": "/var/srs/sound",
    "siprec_mode": "optional"
}
```

* `listen_ip` : The IP address on which the server listens for incoming SIP requests.
* `listen_port` : The port that the server listens on.
* `metadata_path` : Where the metadata of SIPREC calls is stored.
* `sound_path` : Where the sound of SIPREC calls is stored.
* `siprec_mode` :  3 modes for use_siprec:
    * `optional`: In this mode, calls will be established regardless of whether they are SIPREC or not.
    * `mandatory`: In this mode, only SIPREC calls are allowed. If a regular call is attempted, it will result in a "Bad Request" (status code 400) error.
    * `inactive`: In this mode, only regular calls are allowed. If a SIPREC call is attempted, it will result in a "Bad Extension" (status code 420) error.

## Run
### Docker
Build image
```
docker build -t session_recording_server .
```

Run container
```
docker run session_recording_server
```

### Build
#### Install requirment package
```bash
sudo apt install build-essential
```

```bash
sudo apt-get install libasound2-dev
```

```bash
sudo apt-get install libssl-dev
```

```bash
sudo apt install nlohmann-json3-dev
```

```bash
sudo apt install pkg-config 
```

#### Building the source code

```bash
./build.sh
```

#### Run session recording server
Go to `build` directory then run this command
```
./srs
```