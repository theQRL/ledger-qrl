from __future__ import print_function

from binascii import hexlify

from pyledgerqrl import ledgerqrl
from pyledgerqrl.ledgerqrl import *

dev = LedgerQRL()

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
    assert answer[1] == 0
    assert answer[2] == 1
    assert answer[3] == 0

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
    assert answer[1] == 0
    assert answer[2] == 1
    assert answer[3] == 0

def test_comms_HID():
    print()
    dev.U2FMODE = False
    for i in range(0, 260, 10):
        answer = dev.send(INS_TEST_COMM, i)
        assert answer is not None
        assert len(answer) == i
