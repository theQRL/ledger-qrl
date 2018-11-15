# Ledger App for Quantum Resistant Ledger (QRL)

[![CircleCI](https://circleci.com/gh/theQRL/ledger-qrl-private.svg?style=shield&circle-token=422094fd9f5dfcf31b42d41050fb2c7aed42f1df)](https://circleci.com/gh/theQRL/ledger-qrl-private)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f65e5f5f429b46579ae539be28d08695)](https://www.codacy.com/app/ZondaX/ledger-qrl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jleni/ledger-qrl&amp;utm_campaign=Badge_Grade)

This is a prototype of the Ledger Nano S app for Quantum Resistant Ledger. 

It is work in progress and subject to further modifications and testing.

**WARNING: DO NOT USE THIS APP IN A LEDGER WITH REAL FUNDS.**

This repo includes:
- Ledger Nano S app/firmware
- C++ unit tests

# Demo Firmware

Continuous integration generates a demo.zip. This includes firmware plus a shell script that installs the firmware. 

**WARNING**: Again! Remember to use this ONLY in a ledger without funds and only for testing purposes.

# More info

[Build instructions](src/ledger/docs/BUILD.md)

**Specifications**

- [APDU Protocol](src/ledger/docs/PROTOSPEC.md)
- [Transaction format](src/ledger/docs/TXSPEC.md)
