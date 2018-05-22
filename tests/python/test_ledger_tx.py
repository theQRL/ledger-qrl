from __future__ import print_function

from tests.python.pyledgerqrl import ledgerqrl
from tests.python.pyledgerqrl.ledgerqrl import INS_VERSION


def test_version():
    answer = ledgerqrl.send(INS_VERSION)
    assert answer is not None
    assert len(answer) == 4
    assert answer[0] == 0xFF
    assert answer[1] == 0
    assert answer[2] == 0
    assert answer[3] == 2
