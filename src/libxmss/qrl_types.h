// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include <stdint.h>
// QRL TX definitions

#pragma pack(push, 1)
enum qrltx_type {
    QRLTX_TX = 0,
    QRLTX_TXTOKEN = 1,
    QRLTX_SLAVE = 2
};

typedef struct {
    uint8_t address[39];    // address is expected to be 39 bytes
    uint8_t amount[8];      // amount or fee
} qrltx_addr_block;         // 47 bytes

typedef struct {
    uint8_t pk[35];         // ePK is expected to be 35 bytes
    uint8_t access[8];      // access type
} qrltx_slave_block;        // 43 bytes

typedef struct {
    qrltx_addr_block master;    // 47
    qrltx_addr_block dst[4];
} qrltx_tx_t;                   // 235 bytes

typedef struct {
    qrltx_addr_block master;    // 47
    qrltx_addr_block dst[3];    // 47*3
    uint8_t token_hash[32];     // 32
} qrltx_txtoken_t;              // 220 bytes

typedef struct {
    qrltx_addr_block master;    // 47
    qrltx_addr_block qrltx_slave_block[4];
} qrltx_slave_t;                // 219 bytes

typedef struct {
    enum qrltx_type type;
    union {
        qrltx_tx_t tx;
        qrltx_txtoken_t txtoken;
        qrltx_slave_t slave;
    };
} qrltx_t;
#pragma pack(pop)
