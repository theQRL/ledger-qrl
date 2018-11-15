// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#define WOTS_N             32u
#define WOTS_W             16u
#define WOTS_LEN1          64u
#define WOTS_LEN2          3u
#define WOTS_LEN           (WOTS_LEN1 + WOTS_LEN2)
#define WOTS_LEN_HALF      (WOTS_LEN / 2)
#define WOTS_SIGSIZE       (WOTS_N*WOTS_LEN)

#define SZ_SKSEED          48u
#define SZ_PUBSEED         32u

#define XMSS_N             WOTS_N
#define XMSS_W             WOTS_W
#define XMSS_H             8u
#define XMSS_K             2u

#define XMSS_NUM_NODES     (1u << XMSS_H)
#define XMSS_STK_LEVELS    (XMSS_H+1)
#define XMSS_STK_SIZE      (XMSS_STK_LEVELS*WOTS_N)
#define XMSS_NODES_BUFSIZE (XMSS_NUM_NODES*WOTS_N)

#define XMSS_AUTHPATHSIZE  ((XMSS_H-1)*WOTS_N)
#define XMSS_SIGSIZE       (4+32+WOTS_SIGSIZE+XMSS_AUTHPATHSIZE)
#define XMSS_DIGESTSIZE    (2*WOTS_N)
#define XMSS_PKSIZE        (2+WOTS_N)
#define XMSS_SKSIZE        (4+WOTS_N*4)
