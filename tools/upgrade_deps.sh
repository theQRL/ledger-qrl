#!/usr/bin/env bash
SCRIPT_DIR=$(cd $(dirname $0) && pwd)
APP_DIR=$(realpath ${SCRIPT_DIR}/../src/ledger)

echo ${APP_DIR}

# Update ZXLIB

cd ${APP_DIR}
git remote add ledger-zxlib https://github.com/ZondaX/ledger-zxlib.git
git fetch ledger-zxlib
#git subtree add --prefix deps/ledger-zxlib --squash ledger-zxlib/master
git subtree merge --prefix deps/ledger-zxlib --squash ledger-zxlib/master
