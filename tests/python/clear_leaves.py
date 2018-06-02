import time

from pyledgerqrl import ledgerqrl
from pyledgerqrl.ledgerqrl import *

start = time.time()
for i in range(256):
    data = bytearray([i]) + bytearray.fromhex("00"*32)
    answer = ledgerqrl.send(INS_TEST_WRITE_LEAF, data)
    assert len(answer) == 0

answer = ledgerqrl.send(INS_TEST_PK_GEN_1)
