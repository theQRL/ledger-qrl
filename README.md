# ledger-qrl

[![CircleCI](https://circleci.com/gh/jleni/ledger-qrl.svg?style=shield&circle-token=6866f1c72526ab06cc6224262647fe5f4a73c237)](https://circleci.com/gh/jleni/ledger-qrl)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f65e5f5f429b46579ae539be28d08695)](https://www.codacy.com/app/ZondaX/ledger-qrl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jleni/ledger-qrl&amp;utm_campaign=Badge_Grade)

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
