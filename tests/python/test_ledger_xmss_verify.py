from __future__ import print_function

from pyledgerqrl import ledgerqrl
from pyledgerqrl.ledgerqrl import *

from extra.dummy_test_data import expected_leafs_zeroseed
from extra.dummy_test_data import expected_sig_tc0_idx0
from extra.dummy_test_data import expected_sig_tc1_idx5
from extra.dummy_test_data import expected_sig_tc2_idx10

LedgerQRL.U2FMODE = False
LedgerQRL.DEBUGMODE = True


def test_connect():
    dev = LedgerQRL()
    dev.connect()
    dev.print_info()
    assert dev.connected


def test_version():
    """
    Verify the tests are running in the correct version number
    """
    dev = LedgerQRL()
    answer = dev.send(INS_VERSION)
    assert len(answer) == 4
    assert answer[0] == 0xFF
    assert answer[1] == 1
    assert answer[2] == 1
    assert answer[3] == 0


def test_getstate():
    """
    Check uninitialized state
    """
    dev = LedgerQRL()
    dev.connect()
    dev.print_info()

    assert dev.mode == "not initialized"
    assert dev.mode_code == 0
    assert dev._otsindex == 1


def test_INS_TEST_PK_GEN_1():
    """
    Check uninitialized state
    """
    dev = LedgerQRL()
    dev.connect()
    dev.print_info()

    seed = dev.send(ledgerqrl.INS_TEST_PK_GEN_1)
    print(binascii.hexlify(seed))


def test_getseed():
    """
    Check uninitialized state
    """
    dev = LedgerQRL()
    dev.connect()

    dev.print_info()
    print("-------------------------------------------")
    print("This test requires TEST MODE and MOCK_SEED")
    print("-------------------------------------------")

    seed = dev.send(0x89)
    print(binascii.hexlify(seed))

    if dev.test_mode:
        assert seed == bytearray([0] * 48) or seed == bytearray([255] * 48)


def test_read_leaf():
    """
    Expects all leaves to have been generated or uploaded. It compares with known leaves for the test seed
    """
    dev = LedgerQRL()
    dev.connect()
    dev.print_info()

    if dev.test_mode:
        failed = False
        assert len(expected_leafs_zeroseed) == 256
        for i in range(256):
            answer = dev.send(INS_TEST_READ_LEAF, i)
            leaf = binascii.hexlify(answer).upper()
            if len(answer) != 32 or leaf != expected_leafs_zeroseed[i]:
                failed = True
                print("[{:3}] Tested ERR".format(i))
                print(leaf)
                print(expected_leafs_zeroseed[i])
            else:
                print("[{:3}] Tested OK".format(i))

        assert not failed


def test_pk():
    """
    Expects all leaves to have been generated or uploaded.
    It checks with a known public key for the test seed
    """
    dev = LedgerQRL()
    dev.connect()
    dev.print_info()
    assert len(dev.pk_raw) == 67

    pk1 = "000400" \
          "106D0856A5198967360B6BDFCA4976A433FA48DEA2A726FDAF30EA8CD3FAD211" \
          "3191DA3442686282B3D5160F25CF162A517FD2131F83FBF2698A58F9C46AFC5D"

    pk2 = "000400" \
          "9957A84E3985B70D3F06F7F08C30A35DC3A7599DC565469926B783FB4F9066DB" \
          "3BD0FD0BFE74AD1E172CCBC71A3864F1598AB29CA77BEF6826AFAD4BF0828D4C"

    if dev.test_mode:
        assert dev.pk.upper() == pk1 or dev.pk.upper() == pk2

def get_tx(test_idx):
    if test_idx == 0:
        msg = bytearray(
            # header
            [0, 1] +  # type = 0, subitem_count = 1
            # TX
            [0x22] * 39 +  # master.address
            [0] * 8 +  # master.amount
            [0x33] * 39 +  # dest0.address
            [0] * 8  # dest0.amount
        )
        assert len(msg) == 96
        return msg

    if test_idx == 1:
        msg = bytearray(
            # header
            [0, 2] +  # type = 0, subitem_count = 1
            # TX
            [0x22] * 39 +  # master.address
            [0] * 8 +  # master.amount
            [0x33] * 39 +  # dest0.address
            [0] * 8 +  # dest0.amount
            [0x44] * 39 +  # dest1.address
            [1, 0, 0, 0, 0, 0, 0, 0]  # dest1.amount
        )
        assert len(msg) == 143
        return msg

    if test_idx == 2:
        msg = bytearray(
            # header
            [3, 1] +  # type = 0, subitem_count = 1
            # TX
            [0x22] * 39 +  # master.address
            [0] * 8 +  # master.amount
            [i for i in range(80)]  # message
        )
        assert len(msg) == 129
        return msg


def test_digest_idx_0():
    """
    WARNING: This test requires the sk root to be set!!! RUN UPLOAD LEAVES FIRST
    Checks the message digest for an all zeros message
    """
    dev = LedgerQRL()
    dev.connect()
    dev.send(INS_TEST_SETSTATE, APPMODE_READY, 0)

    msg = get_tx(0)

    index = 0
    answer = dev.send(INS_TEST_DIGEST, index, 0, msg)
    answer = binascii.hexlify(answer).upper()
    print(answer)

    assert answer == b"1E1A3DBE81888E3E65E5329E5E7AB67094689E8676F4F1B7ABC293D98243349F" \
                     b"D1F266CCB592D4695045C0BD5F80B66FCD4C14C0B7B98896F80CC2B0B89F3FC5"


def test_sign_idx_0():
    dev = LedgerQRL()
    dev.connect()

    # Sign test case 0 with position 0
    dev.send(INS_TEST_SETSTATE, APPMODE_READY, 0)
    msg = get_tx(0)
    signature = dev.sign(msg)
    assert signature is not None
    signature = signature.decode('ascii')
    assert signature == expected_sig_tc0_idx0


def test_sign_idx_5():
    dev = LedgerQRL()
    dev.connect()

    # Sign test case 1 with position 5
    dev.send(INS_TEST_SETSTATE, APPMODE_READY, 5)
    msg = get_tx(1)
    signature = dev.sign(msg)
    assert signature is not None
    signature = signature.decode('ascii')
    assert signature == expected_sig_tc1_idx5


def test_sign_idx_10():
    dev = LedgerQRL()
    dev.connect()

    # Sign test case 2 with position 10
    dev.send(INS_TEST_SETSTATE, APPMODE_READY, 10)
    msg = get_tx(2)
    signature = dev.sign(msg)
    assert signature is not None
    signature = signature.decode('ascii')
    assert signature == expected_sig_tc2_idx10
