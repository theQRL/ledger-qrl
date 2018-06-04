from pyledgerqrl import ledgerqrl
from pyledgerqrl.ledgerqrl import *

dev = LedgerQRL()
answer = dev.send(INS_TEST_SET_STATE, APPMODE_READY, 5)
