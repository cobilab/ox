/*------------------------------------------------------------------------------

Copyright 2025 IEETA/DETI/University of Aveiro, Portugal.
All Rights Reserved.

These programs are supplied free of charge for research purposes only,
and may not be sold or incorporated into any commercial product. There is
ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are
fit for ANY PURPOSE WHATSOEVER. Use them at your own risk. If you do
happen to find a bug, or have modifications to suggest, please report
the same to Armando J. Pinho, ap@ua.pt. The copyright notice above
and this statement of conditions must remain an integral part of each
and every copy made of these files.

------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "io.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Stream *CreateStream(void) 
  {
  Stream *s;

  if((s = (Stream *) calloc(1, sizeof(Stream))) == NULL) 
    {
    fprintf(stderr, "Error: out of memory\n");
    exit(1);
    }

  return s;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void InitStream(Stream *s, FILE* fp, const char *mode) 
  {
  s->fp = fp;
  s->mode = *mode;
  s->buf_limit = s->buf + IO_BUF_SIZE;
  if(*mode == 'r') 
    { // Open for reading
    s->buf_ptr = s->buf_limit;
    s->size = IO_BUF_SIZE;
    }
  else // Open for writing
    s->buf_ptr = s->buf;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// s->buf_ptr points to the next free position

inline void PutC(int c, Stream *s) 
  {
  *s->buf_ptr++ = c;

  if(s->buf_ptr == s->buf_limit) 
    { // buffer is full: write it
    if(fwrite(s->buf, 1, IO_BUF_SIZE, s->fp) != IO_BUF_SIZE) 
      {
      perror("fwrite");
      exit(1);
      }
    s->buf_ptr = s->buf;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// s->ptr points to the next char

inline int GetC(Stream *s) 
  {
  if(s->buf_ptr == s->buf_limit) 
    { // buffer is empty: get another block
    if((s->size = fread(s->buf, 1, IO_BUF_SIZE, s->fp)) == 0)
      return EOF;
    s->buf_ptr = s->buf;
    } 
  else 
    {
    if(s->buf_ptr - s->buf == s->size)
      return EOF;
    }

  return *s->buf_ptr++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *GetS(char *str, int size, Stream *s) 
  {
  int c, n = 0;

  while(n < size - 1 && (c = GetC(s)) != EOF)
    if((str[n++] = c) == '\n')
      break;

  str[n] = '\0';

  if(n)
    return str;
  else
    return NULL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// s->ptr points to a free position

void FlushStream(Stream *s) 
  {
  size_t n_bytes_to_write = s->buf_ptr - s->buf;

  if(n_bytes_to_write != 0) 
    { // If buf is not empty
    if(fwrite(s->buf, 1, n_bytes_to_write, s->fp) != n_bytes_to_write) 
      {
      perror("fwrite");
      exit(1);
      }
    s->buf_ptr = s->buf;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int SeekSet(Stream *s, off_t offset) 
  {
  if(fseeko(s->fp, offset, SEEK_SET) == -1) 
    {
    perror("fseek");
    exit(1);
    }

  if(s->mode == 'r') 
    { // Open for reading
    s->buf_ptr = s->buf_limit;
    s->size = IO_BUF_SIZE;
    } 
  else
    { // Open for writing
    s->buf_ptr = s->buf;
    }
  
  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

