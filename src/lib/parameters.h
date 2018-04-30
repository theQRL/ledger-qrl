// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#define WOTS_N             32u
#define WOTS_W             16u
#define WOTS_LEN1          64u
#define WOTS_LEN2          3u
#define WOTS_LEN           (WOTS_LEN1 + WOTS_LEN2)
#define WOTS_KEYSIZE       2144u

#define XMSS_H             4u
#define XMSS_K             2u

#define XMSS_NUM_NODES     (1u << XMSS_H)
#define XMSS_STK_LEVELS    (XMSS_H+1)
#define XMSS_STK_SIZE      (XMSS_STK_LEVELS*WOTS_N)
#define XMSS_NODES_BUF_SZ  (XMSS_NUM_NODES*WOTS_N)


