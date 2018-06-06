from __future__ import print_function

from binascii import hexlify

from pyledgerqrl.ledgerqrl import *

dev = LedgerQRL()

EXPECTED_VERSION_MAJOR = 0
EXPECTED_VERSION_MINOR = 2
EXPECTED_VERSION_PATCH = 0

def test_version_U2F():
    print()
    dev.U2FMODE = True

    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    assert answer is not None
    assert len(answer) == 4
    assert answer[0] == 0xFF
    assert answer[1] == EXPECTED_VERSION_MAJOR
    assert answer[2] == EXPECTED_VERSION_MINOR
    assert answer[3] == EXPECTED_VERSION_PATCH

def test_comms_U2F():
    print()
    dev.U2FMODE = True

    for i in range(0, 260, 10):
        answer = dev.send(INS_TEST_COMM, i)
        assert answer is not None
        assert len(answer) == i

def test_version_HID():
    dev.U2FMODE = False
    print()
    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    assert answer is not None
    assert len(answer) == 4
    assert answer[0] == 0xFF
    assert answer[1] == EXPECTED_VERSION_MAJOR
    assert answer[2] == EXPECTED_VERSION_MINOR
    assert answer[3] == EXPECTED_VERSION_PATCH

def test_comms_HID():
    print()
    dev.U2FMODE = False
    for i in range(0, 260, 10):
        answer = dev.send(INS_TEST_COMM, i)
        assert answer is not None
        assert len(answer) == i
