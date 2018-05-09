import time

from tests.python.known_values import expected_leafs_zeroseed
from tests.python.pyledgerqrl import ledgerqrl
from tests.python.pyledgerqrl.ledgerqrl import INS_TEST_WRITE_LEAF, INS_TEST_PK_GEN_1


assert len(expected_leafs_zeroseed) == 256
start = time.time()
for i in range(256):
    data = bytearray([i]) + bytearray.fromhex(expected_leafs_zeroseed[i])
    answer = ledgerqrl.send(INS_TEST_WRITE_LEAF, data)
    assert len(answer) == 0

answer = ledgerqrl.send(INS_TEST_PK_GEN_1)