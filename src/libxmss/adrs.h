// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include <stdint.h>
#include <string.h>
#include "zxmacros.h"

// Based on page 8 - https://www.ietf.org/id/draft-irtf-cfrg-xmss-hash-based-signatures-12.txt
#pragma pack(push, 1)
union ADRS_t {
  // 4   layer        padding       padding         padding
  // 4   tree         padding       padding         padding
  // 4   tree         padding       padding         padding
  // 4   type         padding       padding         padding
  // 4   padding      OTS           ltree           padding
  // 4   padding      chain         height          height
  // 4   padding      hash          index           index
  // 4   keyAndMask   *keyAndMask   *keyAndMask     *keyAndMask

  uint8_t raw[32];

  struct {
    uint32_t layer;
    uint64_t tree;
    uint32_t type;
    uint8_t _padding[12];
    uint32_t keyAndMask;
  };

  struct {
    // type = 0
    uint8_t _padding[16];
    uint32_t OTS;
    uint32_t chain;
    uint32_t hash;
    uint8_t _padding2[4];
  } otshash;

  struct {
    // type = 1 and 2
    uint8_t _padding[16];
    uint32_t ltree;
    uint32_t height;
    uint32_t index;
    uint8_t _padding2[4];
  } trees;
};
#pragma pack(pop)

__INLINE void ADRS_init(union ADRS_t *adrs, uint32_t type) {
    memset(adrs->raw, 32, 0);
    adrs->type = HtoNL(type);
}
