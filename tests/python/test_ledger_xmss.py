from __future__ import print_function

import binascii
import time

from ledgerblue.comm import getDongle
from ledgerblue.commException import CommException

try:
    dongle = getDongle(True)
except CommException as e:
    print(e)
    quit()

INS_VERSION = 0
INS_TEST_PK_GEN_1 = 100
INS_TEST_PK_GEN_2 = 101
INS_TEST_PK_GEN_3 = 102


def ledger_send(cmd, params=None):
    if params is None:
        params = []
    answer = None

    start = time.time()
    try:
        cmd_str = "80{0:02x}".format(cmd)
        for p in params:
            cmd_str = cmd_str + "{0:02x}".format(p)

        answer = dongle.exchange(binascii.unhexlify(cmd_str))
    except CommException as e:
        print("COMMEXC: ", e)
    except Exception as e:
        print("COMMEXC: ", e)
    end = time.time()
    print(end - start)
    return answer


def test_version():
    answer = ledger_send(INS_VERSION)
    assert len(answer) == 4
    assert answer[0] == 0xFF
    assert answer[1] == 0
    assert answer[2] == 0
    assert answer[3] == 1


def test_pk_gen_1():
    answer = ledger_send(INS_TEST_PK_GEN_1)
    assert len(answer) == 132

    idx = binascii.hexlify(answer[0:4]).upper()
    seed = binascii.hexlify(answer[4:36]).upper()
    prf_seed = binascii.hexlify(answer[36:68]).upper()
    pub_seed = binascii.hexlify(answer[68:100]).upper()
    root = binascii.hexlify(answer[100:]).upper()

    print(len(answer))
    print(seed)
    print(prf_seed)
    print(pub_seed)

    assert seed == "EDA313C95591A023A5B37F361C07A5753A92D3D0427459F34C7895D727D62816"
    assert prf_seed == "B3AA2224EB9D823127D4F9F8A30FD7A1A02C6483D9C0F1FD41957B9AE4DFC63A"
    assert pub_seed == "3191DA3442686282B3D5160F25CF162A517FD2131F83FBF2698A58F9C46AFC5D"

def test_pk_gen_2():
    answer = ledger_send(INS_TEST_PK_GEN_2, [0, 0])
    assert len(answer) == 32
    leaf = binascii.hexlify(answer).upper()
    print(leaf)
    assert leaf == "98E68D7AB40D358B5B0F4DF4C86AAE78B444BD50248C02773CF1965FAEA092AE"

    answer = ledger_send(INS_TEST_PK_GEN_2, [0, 10])
    assert len(answer) == 32
    leaf = binascii.hexlify(answer).upper()
    print(leaf)
    assert leaf == "A26468A18D4158E6E92369633F288D049E5C41E0A20A9102ADA3876105AD3CB7"

    answer = ledger_send(INS_TEST_PK_GEN_2, [0, 20])
    assert len(answer) == 32
    leaf = binascii.hexlify(answer).upper()
    print(leaf)
    assert leaf == "8E66C0B26238BC9E12804A83AEF0429E9A666266001A826B5025889B45AE86A3"

    answer = ledger_send(INS_TEST_PK_GEN_2, [0, 30])
    assert len(answer) == 32
    leaf = binascii.hexlify(answer).upper()
    print(leaf)
    assert leaf == "99D010F7F40953E629A70EE6E1A346CDF7949B6DC4A29823A2403615FFEE9EDF"

    answer = ledger_send(INS_TEST_PK_GEN_2, [0, 40])
    assert len(answer) == 32
    leaf = binascii.hexlify(answer).upper()
    print(leaf)
    assert leaf == "D2BAD383B25900503A34FA126ABB19D3AAC6FC110F431929C7EB18E613E101F8"
