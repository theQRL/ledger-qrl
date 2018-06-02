from pyledgerqrl import ledgerqrl
from pyledgerqrl.ledgerqrl import *

answer = ledgerqrl.send(INS_TEST_SET_STATE, bytearray([APPMODE_READY, 0, 0]))

