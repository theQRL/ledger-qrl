// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include <stdint.h>
#include "libxmss/xmss_types.h"
#include "libxmss/qrl_types.h"

#pragma pack(push, 1)
typedef union {
    xmss_sig_ctx_t xmss_sig_ctx;
    qrltx_t qrltx;
} app_ctx_t;
#pragma pack(pop)
