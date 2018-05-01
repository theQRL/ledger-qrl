// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include <memory.h>
#define __INLINE inline __attribute__((always_inline)) static

#ifdef LEDGER_SPECIFIC
#define NVCONST const
#else
#define NVCONST
#endif

#ifdef LEDGER_SPECIFIC
#include "os.h"
#include "cx.h"
__INLINE void nvcpy(NVCONST void *dst, const void *src, uint16_t n)
{
    nvm_write(dst, (void *) src, n);
}
#else
__INLINE void nvcpy(NVCONST void *dst, const void *src, uint16_t n)
{
    memcpy(dst, src, n);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __SWAP(v) (((v) & 0x000000FFu) << 24u | ((v) & 0x0000FF00u) << 8u | ((v) & 0x00FF0000u) >> 8u | ((v) & 0xFF000000u) >> 24u)
#define HtoNL(v) __SWAP( v )
#define NtoHL(v) __SWAP( v )
#else
#define HtoNL(x) (x)
#define NtoHL(x) (x)
#endif
