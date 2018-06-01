from tests.python.pyledgerqrl import ledgerqrl
from tests.python.pyledgerqrl.ledgerqrl import *

answer = ledgerqrl.send(INS_TEST_SET_STATE, bytearray([APPMODE_READY, 0, 0]))

