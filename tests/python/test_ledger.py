from __future__ import print_function

from binascii import hexlify

from pyledgerqrl import ledgerqrl
from pyledgerqrl.ledgerqrl import *


def test_version_U2F():
    print()
    ledgerqrl.U2FMODE = True

    answer = ledgerqrl.send(INS_VERSION)
    print(hexlify(answer))

    answer = ledgerqrl.send(INS_VERSION)
    print(hexlify(answer))

    assert answer is not None
    assert len(answer) == 4
    assert answer[0] == 0xFF
    assert answer[1] == 0
    assert answer[2] == 1
    assert answer[3] == 0

def test_comms_U2F():
    print()
    ledgerqrl.U2FMODE = True

    for i in range(0, 260, 10):
        answer = ledgerqrl.send(INS_TEST_COMM, i)
        assert answer is not None
        assert len(answer) == i

def test_version_HID():
    ledgerqrl.U2FMODE = False
    print()
    answer = ledgerqrl.send(INS_VERSION)
    print(hexlify(answer))

    answer = ledgerqrl.send(INS_VERSION)
    print(hexlify(answer))

    assert answer is not None
    assert len(answer) == 4
    assert answer[0] == 0xFF
    assert answer[1] == 0
    assert answer[2] == 1
    assert answer[3] == 0

def test_comms_HID():
    print()
    ledgerqrl.U2FMODE = False
    for i in range(0, 260, 10):
        answer = ledgerqrl.send(INS_TEST_COMM, i)
        assert answer is not None
        assert len(answer) == i
