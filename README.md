# srs
session record server

## Required
### Install pkg-config
```bash
sudo apt install pkg-config 
```

### libasound-development-package
[link](https://stackoverflow.com/questions/16483635/pjsip-new-call-error-unable-to-find-default-audio-device-pjmedia-eaud-nodef)

This happens when the development packages are not installed. From your error message i can tell that libasound-development-package is not installed. Do as following:
```
sudo apt-get install libasound2-dev
```
Pay attention that pjsip would still fail to set the default audio device since you have done the make as this package was missing. So you need to build Pjsip once again

at pjsip directory do the following respectively :
```
./configure
make dep 
make clean
make
make install
```