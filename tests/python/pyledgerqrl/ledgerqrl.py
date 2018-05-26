from __future__ import print_function

import binascii
import time

from ledgerblue.comm import getDongle
from ledgerblue.commException import CommException

INS_VERSION = 0x00
INS_PUBLIC_KEY = 0x01

INS_TEST_PK_GEN_1 = 0x80
INS_TEST_PK_GEN_2 = 0x81
INS_TEST_PK = 0x82

INS_TEST_WOTS_EXPSEED = 0x90

INS_TEST_WRITE_LEAF = 0x83
INS_TEST_READ_LEAF = 0x84
INS_TEST_DIGEST = 0x85
INS_TEST_SIGN_INIT=0x86
INS_TEST_SIGN_NEXT=0x87


def send(cmd, params=None):
    answer = None
    if params is None:
        params = []

    # ledgerblue.comm.U2FKEY = True

    start = time.time()
    dongle = None
    try:
        dongle = getDongle(True)
        cmd_str = "80{0:02x}".format(cmd)
        for p in params:
            cmd_str = cmd_str + "{0:02x}".format(p)

        answer = dongle.exchange(binascii.unhexlify(cmd_str))
    except CommException as e:
        print("COMMEXC: ", e)
    except Exception as e:
        print("COMMEXC: ", e)
    except BaseException as e:
        print("COMMEXC: ", e)
    finally:
        dongle.close()

    end = time.time()
    print(end - start)
    return answer
