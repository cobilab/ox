/////////////////////////////////////////
// RANGE CODER IS FROM EUGENE SHELWIEN //
// & FURTHER ADAPTED BY JAMES BONFIELD //
// I ONLY HAD MINOR CUSTOM ADAPTATIONS //
// INCLUDING THE C LANG IMPLEMENTATION //
// COPYRIGHT D PRATAS 2025, IEETA/DETI //
// ADDITIONAL CHANGES BY A PINHO, 2025 //
/////////////////////////////////////////

#ifndef RC_H
#define RC_H

#include <stdint.h>
#include <stdlib.h>
#include "io.h"

#define TOP (1 << 24)
#define DO(n) for (int _ = 0; _ < n; _++)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct 
  {
  uint64_t low;
  uint32_t range;
  uint32_t code;
  uint64_t bytes_processed;
  }
RangeCoder;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void rc_start_encode(RangeCoder *rc) 
  {
  rc->low = 0;
  rc->range = (uint32_t)(-1);
  rc->bytes_processed = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void rc_start_decode(RangeCoder *rc, Stream *s) 
  {
  rc->low = 0;
  rc->code = 0;
  rc->range = (uint32_t)(-1);
  DO(8)
    rc->code = (rc->code << 8)|GetC(s);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void rc_finish_encode(RangeCoder *rc, Stream *s) 
  {
  DO(8)
    {
    PutC(rc->low >> 56, s);
    rc->bytes_processed++;
    rc->low <<= 8;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline void rc_encode(RangeCoder *rc, uint32_t alphabet_size, uint32_t 
symbol, uint32_t *cums, Stream *s) 
  {
  uint32_t symbol_cum = cums[symbol];
  uint32_t symbol_freq = cums[symbol + 1] - symbol_cum;

  rc->range /= cums[alphabet_size];
  rc->low += symbol_cum * rc->range;
  rc->range *= symbol_freq;

  while(rc->range < TOP)
    {
    if((uint8_t)((rc->low^(rc->low+rc->range))>>56))
      rc->range = ((uint32_t)(rc->low)|(TOP-1))-(uint32_t)(rc->low);

    PutC(rc->low >> 56, s);
    rc->bytes_processed++;
    rc->low <<= 8;
    rc->range <<= 8;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline void rc_encode256(RangeCoder *rc, uint32_t symbol, uint32_t *cums, 
uint32_t total_freq, Stream *s) 
  {
  uint32_t symbol_cum = cums[symbol];
  uint32_t symbol_freq = cums[symbol + 1] - symbol_cum;

  rc->range /= total_freq;
  rc->low += symbol_cum * rc->range;
  rc->range *= symbol_freq;

  while(rc->range < TOP) 
    {
    if((uint8_t)((rc->low^(rc->low+rc->range))>>56))
      rc->range = ((uint32_t)(rc->low)|(TOP-1))-(uint32_t)(rc->low);

    PutC(rc->low >> 56, s);
    rc->bytes_processed++;
    rc->low <<= 8;
    rc->range <<= 8;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline uint32_t next_largest(uint32_t key, uint32_t *data, uint32_t len)
  {
  uint32_t low  = 0;
  uint32_t high = len;

  while(low <= high)
    {
    uint32_t mid = low + ((high - low) >> 1);
    uint32_t value = data[mid];
    if(value < key)
      low = mid + 1;
    else if(value == key)
      return mid;
    else
      high = mid - 1;
    }

  return (low < high) ? low : high;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline uint32_t rc_decode256(RangeCoder *rc, uint32_t *cums, 
uint32_t total_freq, Stream *s)
  {
  rc->range /= total_freq;
  uint32_t cum = rc->code / rc->range;

  uint32_t symbol = next_largest(cum, cums, 256);

  uint32_t temp = cums[symbol] * rc->range;
  rc->low += temp;
  rc->code -= temp;
  rc->range *= (cums[symbol+1] - cums[symbol]);

  while(rc->range < TOP)
    {
    if((uint8_t)((rc->low^(rc->low+rc->range))>>56))
      rc->range = ((uint32_t)(rc->low)|(TOP-1))-(uint32_t)(rc->low);

    rc->code = (rc->code<<8)|GetC(s);
    rc->range <<= 8;
    rc->low <<= 8;
    }

  return symbol;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline uint32_t rc_decode(RangeCoder *rc, uint32_t alphabet_size, 
uint32_t *cums, Stream *s) 
  {
  rc->range /= cums[alphabet_size];
  uint32_t cum = rc->code / rc->range;
  
  uint32_t symbol = next_largest(cum, cums, alphabet_size);

  uint32_t temp = cums[symbol] * rc->range;
  rc->low += temp;
  rc->code -= temp;
  rc->range *= (cums[symbol+1] - cums[symbol]);

  while(rc->range < TOP) 
    {
    if((uint8_t)((rc->low^(rc->low+rc->range))>>56))
      rc->range = ((uint32_t)(rc->low)|(TOP-1))-(uint32_t)(rc->low);

    rc->code = (rc->code<<8)|GetC(s);
    rc->range <<= 8;
    rc->low <<= 8;
    }

  return symbol;
  } 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

