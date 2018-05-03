#!/usr/bin/env python
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


def send(cmd, params=[]):
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
    print(end-start)
    return answer

answer = send(101)

