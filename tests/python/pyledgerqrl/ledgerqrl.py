from __future__ import print_function

import binascii
import time

from ledgerblue import commU2F, comm
from ledgerblue.commException import CommException

CLA                     =0x77

INS_VERSION             =0x00
INS_GETSTATE            =0x01
INS_KEYGEN              =0x02
INS_PUBLIC_KEY          =0x03
INS_SIGN                =0x04
INS_SIGN_NEXT           =0x05

INS_TEST_PK_GEN_1 = 0x80
INS_TEST_PK_GEN_2 = 0x81

INS_TEST_WRITE_LEAF = 0x83
INS_TEST_READ_LEAF = 0x84
INS_TEST_DIGEST = 0x85
INS_TEST_SET_STATE=0x88
INS_TEST_COMM = 0x89

last_error = 0

APPMODE_NOT_INITIALIZED=0x00
APPMODE_KEYGEN_RUNNING =0x01
APPMODE_READY          =0x02

SCRAMBLE_KEY = "QRL"
U2FMODE = True

def send(ins, p1=0, p2=0, params=None):
    global last_error

    answer = None
    if params is None:
        params = []

    params = bytearray([len(params)])+ bytearray(params)

    start = time.time()
    dongle = None
    try:
        if U2FMODE:
            dongle = commU2F.getDongle(scrambleKey=SCRAMBLE_KEY, debug=True)
        else:
            dongle = comm.getDongle(debug=True)

        cmd_str = "{0:02x}{1:02x}{2:02x}{3:02x}".format(CLA, ins, p1, p2)
        for p in params:
            cmd_str = cmd_str + "{0:02x}".format(p)

        answer = dongle.exchange(binascii.unhexlify(cmd_str))
    except CommException as e:
        print("COMMEXC: ", e)
        last_error = e.sw
    except Exception as e:
        print("Exception: ", e)
    except BaseException as e:
        print("BaseException: ", e)
    finally:
        dongle.close()

    if U2FMODE:
        if answer is not None:
            print("U2F[{}]: {}".format(len(answer), binascii.hexlify(answer)))
            answer=answer[5:]

    end = time.time()
    print(end - start)
    return answer
