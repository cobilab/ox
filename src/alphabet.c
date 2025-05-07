#include "mem.h"
#include "alphabet.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ALPHABET *CreateAlphabet(void)
  {
  ALPHABET *A = (ALPHABET *) Calloc(1, sizeof(ALPHABET));
  A->toChars  = (uint8_t  *) Calloc(256, sizeof(uint8_t));
  A->revMap   = (uint8_t  *) Calloc(256, sizeof(uint8_t));
  A->mask     = (uint8_t  *) Calloc(256, sizeof(uint8_t));
  A->length      = 0;
  A->cardinality = 0;
  return A;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LoadAlphabet(ALPHABET *A, FILE *F)
  {
  uint64_t size = 0;
  uint32_t x;

  uint8_t buf[BUFFER_SIZE];

  size_t bytes_read;
  while((bytes_read = fread(buf, 1, BUFFER_SIZE, F)) > 0)
    for(x = 0 ; x < bytes_read ; ++x)
      {
      A->mask[buf[x]] = 1;
      ++size;
      }
  A->length = size;

  A->cardinality = 0;
  for(x = 0 ; x < 256 ; ++x)
    if(A->mask[x] == 1)
      {
      A->toChars[A->cardinality] = x;
      A->revMap[x] = A->cardinality++;
      }
    else
      A->revMap[x] = 255;

  rewind(F);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveAlphabet(ALPHABET *A)
  {
  Free(A->mask);
  Free(A->revMap);
  Free(A->toChars);
  Free(A);	  
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
