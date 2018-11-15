#QRL App for Ledger Nano S

## Dependencies

### Ledger Nano S

This project requires ledger firmware 1.4.2

### Docker CE

Install docker CE following this instructions:

https://docs.docker.com/install/

### CircleCI CLI

CircleCI allows compiling BOLOS firmware both in Linux and MacOS. The CLI will download a docker container ready to run.

To install, follow the instructions here:

https://circleci.com/docs/2.0/local-cli/#installing-the-circleci-local-cli-on-macos-and-linux-distros

### Ledger Python Tools

Ledger firmware 1.4.2 requires ledgerblue>0.1.18. In most cases, `nanocli.sh` should be able to install all dependencies. You may need to enter your password. 

```bash
./nanocli.sh config
```
It is possible that due to recent changes to the Ledger firmware/SDK some additional steps might be required.

### Ubuntu Dependencies
Install the following packages:
```
sudo apt update && apt-get -y install build-essential git sudo wget cmake libssl-dev libgmp-dev autoconf libtool
```

### OSX Dependencies
It is recommended that you install brew and xcode. 

Additionally you will need to:


```
brew install libusb
```
# Building
There are different local builds:

 - Generic C++ code and run unit tests
 - BOLOS firmware

## Generic C++ Code / Tests

This is useful when you want to make changes to libraries, run unit tests, etc. It will build all common libraries and unit tests.

**Compile**
```
cmake . && make
```
**Run unit tests**
```
export GTEST_COLOR=1 && ctest -VV
```

## BOLOS / Ledger firmware
In order to keep builds reproducible, a bash script is provided.
 The script will build the firmware in a docker container and leave the binary in the correct directory.

**Build**

The following command will build the app firmware inside a container. All output will be available to the host.
```
./nanocli.sh umake
```

**Upload the app to the device**
The following command will upload the application to the ledger. _Warning: The application will be deleted before uploading._
```
./nanocli.sh uload
```

## Continuous Integration (debugging CI issues)
This will build in a docker image identical to what CircleCI uses. This provides a clean, reproducible environment. It also can be helpful to debug CI issues.

**To build in ubuntu 16.04 and run C++ unit tests**
```
circleci build
```

**To build BOLOS firmware**
```
circleci build --job build_ledger
```

**To build go client**
```
circleci build --job build_go
```
