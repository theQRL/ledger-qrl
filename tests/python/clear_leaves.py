import time

from tests.python.pyledgerqrl import ledgerqrl
from tests.python.pyledgerqrl.ledgerqrl import INS_TEST_WRITE_LEAF, INS_TEST_PK_GEN_1

start = time.time()
for i in range(256):
    data = bytearray([i]) + bytearray.fromhex("00"*32)
    answer = ledgerqrl.send(INS_TEST_WRITE_LEAF, data)
    assert len(answer) == 0

answer = ledgerqrl.send(INS_TEST_PK_GEN_1)
