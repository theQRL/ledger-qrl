from __future__ import print_function

from binascii import hexlify

from pyledgerqrl.ledgerqrl import *

dev = LedgerQRL()

EXPECTED_VERSION_MAJOR = 0
EXPECTED_VERSION_MINOR = 3
EXPECTED_VERSION_PATCH = 1


def test_comms_u2f():
    print()
    dev.U2FMODE = True

    for i in range(0, 260, 10):
        # TODO: Fix U2F Ledger's python implementation
        answer = dev.send(INS_TEST_COMM, i)
        assert answer is not None
        assert len(answer) == i


def test_comms_hid():
    print()
    dev.U2FMODE = False
    for i in range(0, 260, 10):
        answer = dev.send(INS_TEST_COMM, i)
        assert answer is not None
        assert len(answer) == i


def test_version_u2f():
    print()
    dev.U2FMODE = True

    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    assert answer is not None
    assert len(answer) == 4
    assert answer[0] == 0xFF
    check_version(EXPECTED_VERSION_MAJOR,
                  EXPECTED_VERSION_MINOR,
                  EXPECTED_VERSION_PATCH)


def test_version_hid():
    dev.U2FMODE = False
    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    answer = dev.send(INS_VERSION)
    print(hexlify(answer))

    assert answer is not None
    assert len(answer) == 4
    assert answer[0] == 0xFF
    check_version(EXPECTED_VERSION_MAJOR,
                  EXPECTED_VERSION_MINOR,
                  EXPECTED_VERSION_PATCH)
