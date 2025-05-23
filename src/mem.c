#include "mem.h"
#include <stdlib.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void *Malloc(size_t size){

  void *pointer = malloc(size);

  if(pointer == NULL){
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
    }
  
  return pointer;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void *Calloc(size_t nmemb, size_t size){

  void *pointer = calloc(nmemb, size);

  if(pointer == NULL){
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
    }

  return pointer;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void *Realloc(void *ptr, size_t size){
  
  void *pointer = realloc(ptr, size);
	
  if(pointer == NULL){
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
    }

  return pointer;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Free(void *ptr){
  free(ptr);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
