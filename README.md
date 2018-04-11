# ledger-qrl

[![CircleCI](https://circleci.com/gh/jleni/ledger-qrl.svg?style=svg&circle-token=6866f1c72526ab06cc6224262647fe5f4a73c237)](https://circleci.com/gh/jleni/ledger-qrl)

This is a basic U2F example project.

Based on [ledger-ci-base](https://github.com/jleni/ledger-ci-base).


## Building

Installing dependencies
```bash
./nanocli.sh config
```  

Building ledger binaries
```bash
./nanocli.sh make
```  

Upload firmware
```bash
./nanocli.sh load
```  

Delete firmware
```bash
./nanocli.sh delete
```  
