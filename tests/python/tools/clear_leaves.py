from pyledgerqrl.ledgerqrl import *

dev = LedgerQRL()

start = time.time()
for i in range(256):
    data = bytearray([i]) + bytearray.fromhex("00" * 32)
    answer = dev.send(INS_TEST_WRITE_LEAF, data)
    assert len(answer) == 0

answer = dev.send(INS_TEST_PK_GEN_1)
