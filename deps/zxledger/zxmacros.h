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
#endif

__INLINE void nvcpy(void *dst, void *src, uint16_t n)
{
#ifdef LEDGER_SPECIFIC
    nvm_write(dst, src, n);
#else
    memcpy(dst, src, n);
#endif
}
__INLINE void nvset(void *dst, uint32_t val)
{
#ifdef LEDGER_SPECIFIC
    nvm_write(dst, (void *) &val, 4);
#else
    *((uint32_t*)dst) = val;
#endif
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __SWAP(v) (((v) & 0x000000FFu) << 24u | ((v) & 0x0000FF00u) << 8u | ((v) & 0x00FF0000u) >> 8u | ((v) & 0xFF000000u) >> 24u)
#define HtoNL(v) __SWAP( v )
#define NtoHL(v) __SWAP( v )
#else
#define HtoNL(x) (x)
#define NtoHL(x) (x)
#endif