#ifndef ALPHABET_H_INCLUDED
#define ALPHABET_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "misc.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint8_t   *mask;
  uint8_t   *revMap;
  uint8_t   *toChars;
  uint32_t  length;
  uint32_t  cardinality;
  }
ALPHABET;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ALPHABET *CreateAlphabet (void);
void     LoadAlphabet    (ALPHABET *, FILE *);
void     RemoveAlphabet  (ALPHABET *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
