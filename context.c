#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "mem.h"
#include "context.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetCModel(CModel *M)
  {
  M->pModelIdx = 0;	
  if(M->mode == HASH_TABLE_MODE)
    {
    RemoveHashTable(M->HT);
    M->HT = CreateHashTable(M->nSym);
    }
  else
    ResetArrayTable(M->AT);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CModel *CreateCModel(uint32_t ctx, uint32_t aDen, uint32_t nSym)
  {
  CModel *M = (CModel *) Calloc(1, sizeof(CModel));
  uint64_t prod = 1, *mult;
  double tmp;

  M->nSym        = nSym;
  mult           = (uint64_t *) Calloc(ctx, sizeof(uint64_t));
  tmp            = pow(M->nSym, ctx);
  M->nPModels    = (uint64_t) tmp;
  M->ctx         = ctx;
  M->alphaDen    = aDen;
  M->pModelIdx   = 0;

  if(tmp * (double)M->nSym * (double)sizeof(ACC) / pow(2,20) > 
  MAX_ARRAY_MEMORY || tmp >= UINT64_MAX)
    {
    M->mode = HASH_TABLE_MODE;
    M->HT   = CreateHashTable(M->nSym);
    }
  else
    {
    M->mode = ARRAY_MODE;
    M->AT   = CreateArrayTable(M->nSym, M->nPModels);
    }

  for(uint32_t n = 0 ; n < M->ctx ; ++n)
    {
    mult[n] = prod;
    prod *= M->nSym;
    }

  M->multiplier = mult[M->ctx-1];
  Free(mult);
  
  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetCModelIdx(CModel *M)
  {
  M->pModelIdx = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetPModelIdx(uint8_t *p, CModel *M)
  {
  M->pModelIdx = ((M->pModelIdx-*(p-M->ctx)*M->multiplier)*M->nSym)+*p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputePModel(CModel *M, PModel *P, uint64_t idx, uint32_t aDen)
  {
  ACC *ac;
  HCC *hc;
  uint32_t x;
  switch(M->mode){
    case HASH_TABLE_MODE:
      if(!(hc = GetHCCounters(M->HT, idx)))
        hc = (HCC *) M->HT->zeroCounters;
      P->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x){
        P->freqs[x] = 1 + aDen * hc[x];
        P->sum += P->freqs[x];
        }
    break;

    case ARRAY_MODE:
      ac = &M->AT->counters[idx*M->nSym];
      P->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x){
        P->freqs[x] = 1 + aDen * ac[x];
        P->sum += P->freqs[x];
        }
    break;

    default:
    fprintf(stderr, "Error: not implemented!\n");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateCModelCounter(CModel *M, uint32_t sym, uint64_t idx)
  {
  if(M->mode == HASH_TABLE_MODE) 
    UpdateHashCounter(M->HT, sym, idx);
  else
    UpdateArrayCounter(M->AT, sym, idx);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveCModel(CModel *M)
  {
  if(M->mode == HASH_TABLE_MODE) 
    RemoveHashTable (M->HT);
  else 
    RemoveArrayTable(M->AT);

  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double PModelSymbolNats(PModel *P, uint32_t s)
  {
  return log((double) P->sum / P->freqs[s]);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
