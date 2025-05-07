#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include "defs.h"
#include "buffer.h"
#include "hash.h"
#include "array.h"
#include "pmodels.h"

#define ARRAY_MODE         0
#define HASH_TABLE_MODE    1
#define MAX_ARRAY_MEMORY   2048    //Size in MBytes

typedef struct
  {
  uint32_t   ctx;           // Current depth of context template
  uint64_t   nPModels;      // Maximum number of probability models
  uint32_t   alphaDen;      // Denominator of alpha
  uint32_t   maxCount;      // Counters /= 2 if one counter >= maxCount
  uint64_t   multiplier;
  uint32_t   mode;
  HASH       *HT;
  ARRAY      *AT;
  uint64_t   pModelIdx;     // IDX
  uint32_t   nSym;          // EDITS
  }
CModel;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void            GetPModelIdx         (uint8_t *, CModel *);
void            ResetCModelIdx       (CModel *);
void            ResetCModel          (CModel *);
void            UpdateCModelCounter  (CModel *, uint32_t, uint64_t);
CModel          *CreateCModel        (uint32_t, uint32_t, uint32_t);
void            ComputePModel        (CModel *, PModel *, uint64_t, uint32_t);
void            RemoveCModel         (CModel *);
double          PModelSymbolNats     (PModel *, uint32_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
