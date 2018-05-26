from tests.python.known_values import expected_leafs_zeroseed
from tests.python.pyledgerqrl import ledgerqrl
from tests.python.pyledgerqrl.ledgerqrl import *

##########################3
# KEYGEN PHASE 1
answer = ledgerqrl.send(INS_TEST_PK_GEN_1)
idx = binascii.hexlify(answer[0:4]).upper()
seed = binascii.hexlify(answer[4:36]).upper()
prf_seed = binascii.hexlify(answer[36:68]).upper()
pub_seed = binascii.hexlify(answer[68:100]).upper()
root = binascii.hexlify(answer[100:]).upper()

print(len(answer))
print(seed)
print(prf_seed)
print(pub_seed)

assert seed == "EDA313C95591A023A5B37F361C07A5753A92D3D0427459F34C7895D727D62816"
assert prf_seed == "B3AA2224EB9D823127D4F9F8A30FD7A1A02C6483D9C0F1FD41957B9AE4DFC63A"
assert pub_seed == "3191DA3442686282B3D5160F25CF162A517FD2131F83FBF2698A58F9C46AFC5D"

##########################3
# KEYGEN PHASE 2 - UPLOAD
assert len(expected_leafs_zeroseed) == 256
start = time.time()
for i in range(0, 256, 4):
    print("====", i)

    data = bytearray([i]) + \
           bytearray.fromhex(expected_leafs_zeroseed[i+0]) + \
           bytearray.fromhex(expected_leafs_zeroseed[i+1]) + \
           bytearray.fromhex(expected_leafs_zeroseed[i+2]) + \
           bytearray.fromhex(expected_leafs_zeroseed[i+3])

    answer = ledgerqrl.send(INS_TEST_WRITE_LEAF, data)
    assert len(answer) == 0

#########################3
#KEYGEN PHASE 3
answer = ledgerqrl.send(INS_TEST_SET_STATE, bytearray([APPMODE_READY, 0, 0]))

answer = ledgerqrl.send(INS_PUBLIC_KEY, [])
assert len(answer) == 64
leaf = binascii.hexlify(answer).upper()
print(leaf)
assert leaf == "106D0856A5198967360B6BDFCA4976A433FA48DEA2A726FDAF30EA8CD3FAD211" \
               "3191DA3442686282B3D5160F25CF162A517FD2131F83FBF2698A58F9C46AFC5D"
