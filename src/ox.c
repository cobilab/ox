#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
#include <sys/stat.h>
#include "defs.h"
#include "io.h"
#include "rc.h"
#include "mem.h"
#include "msg.h"
#include "misc.h"
#include "alphabet.h"
#include "context.h"

#define PLOT_HEIGHT 12
#define PLOT_WIDTH  120

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Fread(void *ptr, size_t size, size_t count, FILE *stream)
  {
  if(fread(ptr, size, count, stream) != count)
    {
    fprintf(stderr, "Error: fread failed.\n");
    exit(EXIT_FAILURE);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Fwrite(void *ptr, size_t size, size_t count, FILE *stream)
  {
  if(fwrite(ptr, size, count, stream) == (size_t) -1)
    {
    fprintf(stderr, "Error: fwrite failed.\n");
    exit(EXIT_FAILURE);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t Get_file_size(const char *fn)
  {
  struct stat st;
  if(stat(fn, &st) == 0)
    return (uint64_t)st.st_size;
  else
    perror("stat failed");
  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t ECH(char c) 
  {
  switch(c) 
    {
    case 'A': return 0;
    case 'B': return 1;
    case 'C': return 2;
    case 'D': return 3;
    default: return 0;  
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char DCH(uint8_t v) 
  {
  switch(v) 
    {
    case 0: return 'A';
    case 1: return 'B';
    case 2: return 'C';
    case 3: return 'D';
    default: return 'A';  // fallback
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t ECH4(char c) 
  {
  if(c >= 'A' && c <= 'P') 
    return c - 'A';
  return 0; // fallback
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char DCH4(uint8_t val) // Convert 4-bit value to character 'A'-'P'
  {
  if(val < 16)
    return 'A' + val;
  return 'A'; // fallback
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t Pack_four_chars(uint8_t *ch) 
  {
  return (ECH(ch[0])<<6)|(ECH(ch[1])<<4)|(ECH(ch[2])<<2)|(ECH(ch[3]));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Unpack_four_chars(uint8_t byte, char *ch) 
  {
  ch[0] = DCH((byte>>6)&0x03);
  ch[1] = DCH((byte>>4)&0x03);
  ch[2] = DCH((byte>>2)&0x03);
  ch[3] = DCH(byte&0x03);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t Pack_two_chars(char a, char b) 
  {
  return (ECH4(a)<<4)|ECH4(b);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Unpack_byte_to_chars(uint8_t byte, char *a, char *b) 
  {
  *a = DCH4((byte >> 4) & 0x0F);
  *b = DCH4(byte & 0x0F);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Generate_seq(FILE *OUT, uint64_t size, int32_t cardinality, int32_t seed) 
  {
  if(seed == -1)
    srand((uint32_t)time(NULL));
  else
    srand((uint32_t)seed);

  uint8_t buffer[BUFFER_SIZE];
  uint64_t i = 0;

  while(i < size)
    {
    uint64_t chunk_size = (size - i > BUFFER_SIZE) ? BUFFER_SIZE : (size - i);
    for(uint64_t j = 0 ; j < chunk_size ; ++j)
      buffer[j] = (uint8_t)(rand() % cardinality);
    fwrite(buffer, 1, chunk_size, OUT);
    i += chunk_size;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Complexity_Profile(int argc, char *argv[])
  {
  int32_t ctx = 3;
  uint32_t alphaDen = 1;
  int32_t window_size = 2;

  if(argc <= 2)
    {
    print_profile_usage(argv[0]);
    exit(1);
    }

  optind = 2;
  int opt;
  while((opt = getopt(argc, argv, "k:a:w:")) != -1)
    {
    switch(opt)
      {
      case 'k':
        ctx = atoi(optarg);
      break;
      case 'a':
        alphaDen = atoi(optarg);
      break;
      case 'w':
        window_size = atoi(optarg);
      break;
      default:
        print_profile_usage(argv[0]);
        exit(1);
      }
    }

  // Remaining argument should be the output file
  if(optind >= argc || ctx == 0 || alphaDen <= 0)
    {
    print_profile_usage(argv[0]);
    exit(1);
    }

  fprintf(stderr, "Context (k): %u\n", ctx);
  fprintf(stderr, "AlphaDen (a): %u\n", alphaDen);
  fprintf(stderr, "Window size (w): %u\n", window_size);

  FILE *IN = Fopen(argv[argc-1], "rb");

  ALPHABET *AL = CreateAlphabet();
  LoadAlphabet(AL, IN);

  fprintf(stderr, "Alphabet cardinality: %u\n", AL->cardinality);

  CModel *CM = CreateCModel(ctx, alphaDen, AL->cardinality);
  CBUF   *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  PModel *PM = CreatePModel(AL->cardinality);

  int sym;
  double bits = 0;
  double ic = 0;
  uint64_t sequence_size = 0;
  uint8_t buf[BUFFER_SIZE];
  size_t bytes_read;

  // Moving average buffers
  double *ic_window = malloc(window_size * sizeof(double));
  size_t ic_index = 0;

  double *smoothed_ic = malloc(1000000 * sizeof(double));  // should be enough
  size_t smoothed_len = 0;

  while((bytes_read = fread(buf, 1, BUFFER_SIZE, IN)) > 0) 
    for(size_t i = 0 ; i < bytes_read ; ++i) 
      {
      symBuf->buf[symBuf->idx] = sym = AL->revMap[buf[i]];
      GetPModelIdx(&symBuf->buf[symBuf->idx-1], CM);
      ComputePModel(CM, PM, CM->pModelIdx, CM->alphaDen);
      ic = PModelSymbolNats(PM, sym) / M_LN2;
      bits += ic;
      UpdateCModelCounter(CM, sym, CM->pModelIdx);
      UpdateCBuffer(symBuf);
      ++sequence_size;

      // Moving average smoothing
      ic_window[ic_index % window_size] = ic;
      ic_index++;
      size_t count = (ic_index < window_size) ? ic_index : window_size;
      double sum = 0;
      for(size_t j = 0 ; j < count ; ++j) 
        sum += ic_window[j];
      double avg_ic = sum / count;
      smoothed_ic[smoothed_len++] = avg_ic;
      }

  RemovePModel(PM);
  RemoveCBuffer(symBuf);

  fprintf(stderr, "NC: %lf\n", bits / ((double) sequence_size * 
  log2(AL->cardinality)));

  fclose(IN);

  // Complexity profile plotting
  // Downsample to PLOT_WIDTH
  double plot_data[PLOT_WIDTH];
  size_t step = smoothed_len / PLOT_WIDTH;
  if (step < 1) step = 1;

  size_t plot_len = 0;
  for(size_t i = 0 ; i < smoothed_len && plot_len < PLOT_WIDTH ; i += step)
    plot_data[plot_len++] = smoothed_ic[i];

  // Find min and max
  double max_val = plot_data[0], min_val = plot_data[0];
  for(size_t i = 1 ; i < plot_len ; ++i) 
    {
    if(plot_data[i] > max_val) 
      max_val = plot_data[i];
    if(plot_data[i] < min_val) 
      min_val = plot_data[i];
    }

  // Create canvas
  char canvas[PLOT_HEIGHT][PLOT_WIDTH];
  memset(canvas, ' ', sizeof(canvas));

  for(size_t x = 0 ; x < plot_len ; ++x) 
    {
    double norm = (plot_data[x] - min_val) / (max_val - min_val + 1e-9);
    int y = PLOT_HEIGHT - 1 - (int)(norm * (PLOT_HEIGHT - 1));
    if(y < 0) 
      y = 0;
    if(y >= PLOT_HEIGHT) 
      y = PLOT_HEIGHT - 1;
    canvas[y][x] = '*';
    }

  for (int y = 0 ; y < PLOT_HEIGHT ; ++y)
    {
    for (size_t x = 0; x < plot_len; ++x)
      putchar(canvas[y][x]);
    putchar('\n');
    }

  free(ic_window);
  free(smoothed_ic);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Generate_sequence(int argc, char *argv[])
  {
  int32_t seed = -1;
  int32_t cardinality = 0;
  uint64_t size = 100;
  char *outfile = NULL;

  if(argc <= 2) 
    {
    print_generate_usage(argv[0]);
    exit(1);
    }

  optind = 2;
  int opt;
  while((opt = getopt(argc, argv, "s:c:e:")) != -1) 
    {
    switch(opt) 
      {
      case 's':
        size = strtoull(optarg, NULL, 10);
      break;
      case 'c':
        cardinality = atoi(optarg);
      break;
      case 'e':
        seed = atoi(optarg);
      break;
      default:
        print_generate_usage(argv[0]);
        exit(1);
      }
    }

  if(optind >= argc || size == 0 || cardinality <= 0 || cardinality > 256) 
    {
    print_generate_usage(argv[0]);
    exit(1);
    }

  outfile = argv[optind];

  FILE *OUT = Fopen(outfile, "wb");

  Generate_seq(OUT, size, cardinality, seed);
  
  fclose(OUT);

  fprintf(stderr, "Generated sequence with %"PRIu64" symbols"
  " and cardinality %d into %s\n", size, cardinality, outfile);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Pack_file_2bit(const char *in_path, const char *out_path)
  {
  FILE *IN = Fopen(in_path, "rb");
  FILE *OUT = Fopen(out_path, "wb");

  uint8_t in_buf[BUFFER_SIZE];
  uint8_t out_buf[BUFFER_SIZE / 4];
  
  uint64_t file_size = Get_file_size(in_path);
  Fwrite(&file_size, sizeof(uint64_t), 1, OUT);

  size_t bytes_read;
  while((bytes_read = fread(in_buf, 1, BUFFER_SIZE, IN)) > 0) 
    {
    size_t out_index = 0;
    for(size_t i = 0 ; i < bytes_read ; i += 4) 
      {
      uint8_t group[4] = {0};
      // Copy up to 4 chars, pad with 'A' if necessary
      for(int j = 0 ; j < 4 && (i + j) < bytes_read ; ++j) 
        group[j] = in_buf[i + j];
      out_buf[out_index++] = Pack_four_chars(group);
      }
    Fwrite(out_buf, 1, out_index, OUT);
    }

  fclose(IN);
  fclose(OUT);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Pack_file_4bit(const char *in_path, const char *out_path)
  {
  FILE *IN = Fopen(in_path, "rb");
  FILE *OUT = Fopen(out_path, "wb"); 

  char in_buf[BUFFER_SIZE];
  uint8_t out_buf[BUFFER_SIZE / 2];

  uint64_t file_size = Get_file_size(in_path);
  Fwrite(&file_size, sizeof(uint64_t), 1, OUT);

  size_t bytes_read;
  while((bytes_read = fread(in_buf, 1, BUFFER_SIZE, IN)) > 0) 
    {
    size_t out_index = 0;
    for (size_t i = 0 ; i < bytes_read ; i += 2) 
      {
      char a = in_buf[i];
      char b = (i + 1 < bytes_read) ? in_buf[i + 1] : 'A'; // pad with 'A'
      out_buf[out_index++] = Pack_two_chars(a, b);
      }
    fwrite(out_buf, 1, out_index, OUT);
    }

  fclose(IN);
  fclose(OUT);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Unpack_file_2bit(const char *in_path, const char *out_path)
  {
  FILE *IN = Fopen(in_path, "rb");
  FILE *OUT = Fopen(out_path, "wb");

  uint8_t in_buf[BUFFER_SIZE / 4];
  char out_buf[BUFFER_SIZE];

  uint64_t original_size = 0;
  Fread(&original_size, sizeof(uint64_t), 1, IN);

  size_t bytes_read;
  uint64_t total_written = 0;

  while((bytes_read = fread(in_buf, 1, BUFFER_SIZE / 4, IN)) > 0) 
    {
    size_t out_index = 0;
    for(size_t i = 0 ; i < bytes_read ; ++i) 
      {
      char group[4];
      Unpack_four_chars(in_buf[i], group);

      for(int j = 0 ; j < 4 && total_written < original_size ; ++j) 
        {
        out_buf[out_index++] = group[j];
        ++total_written;
        }
      }
    Fwrite(out_buf, 1, out_index, OUT);
    }

  fclose(IN);
  fclose(OUT);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Unpack_file_4bit(const char *in_path, const char *out_path)
  {
  FILE *IN = Fopen(in_path, "rb");
  FILE *OUT = Fopen(out_path, "wb");
  
  uint8_t in_buf[BUFFER_SIZE / 2];
  char out_buf[BUFFER_SIZE];

  uint64_t original_size = 0;
  Fread(&original_size, sizeof(uint64_t), 1, IN);

  size_t bytes_read;
  uint64_t total_written = 0;

  while((bytes_read = fread(in_buf, 1, BUFFER_SIZE / 2, IN)) > 0) 
    {
    size_t out_index = 0;
    for(size_t i = 0 ; i < bytes_read && total_written < original_size ; ++i) 
      {
      char a, b;
      Unpack_byte_to_chars(in_buf[i], &a, &b);
      if(total_written < original_size) 
        out_buf[out_index++] = a, ++total_written;
      if(total_written < original_size) 
        out_buf[out_index++] = b, ++total_written;
      }
    Fwrite(out_buf, 1, out_index, OUT);
    }

  fclose(IN);
  fclose(OUT);
  } 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Print_cum_frequencies(uint32_t *cumFreqs)
  {
  for(size_t i = 0 ; i < 257 ; ++i)
    fprintf(stderr, "%u ", cumFreqs[i]);
  fprintf(stderr, "\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Compute_cumulative_frequencies(uint64_t *freqs, uint32_t *cumFreqs)
  {
  cumFreqs[0] = 0;
  for(size_t i = 0 ; i < 256 ; ++i)
    cumFreqs[i+1] = cumFreqs[i] + freqs[i];

  size_t len = 257;
  uint64_t min = cumFreqs[0];
  uint64_t max = cumFreqs[len-1];

  for(size_t i = 0 ; i < len ; ++i) // Normalize to [0, 65535]
    cumFreqs[i] = (uint32_t)(((uint64_t)(cumFreqs[i]-min)*65535)/(max-min));

  for(size_t i = 1 ; i < len ; i++) // Ensure uniqueness by adjusting duplicates
    if(cumFreqs[i] <= cumFreqs[i-1])
      cumFreqs[i] = cumFreqs[i-1] + 1;

  Print_cum_frequencies(cumFreqs);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Build_frequency_table(const char *in, uint64_t *freqs, uint32_t *cumFreqs)
  {
  FILE *F = Fopen(in, "rb");
  uint8_t buffer[BUFFER_SIZE];

  memset(freqs, 0, sizeof(uint32_t) * 256);

  size_t read;
  while((read = fread(buffer, 1, BUFFER_SIZE, F)) > 0)
    for(size_t i = 0 ; i < read ; ++i)
      freqs[buffer[i]]++;

  fclose(F);
  
  for(size_t i = 0 ; i < 256 ; ++i)
    freqs[i] += 1;

  Compute_cumulative_frequencies(freqs, cumFreqs);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Crc32_from_file(const char *fn)
  {
  static uint32_t table[256];
  static int table_computed = 0;
    
  if(!table_computed)
    {
    for(uint32_t i = 0 ; i < 256 ; ++i)
      {
      uint32_t crc = i;
      for(size_t j = 0 ; j < 8 ; ++j)
        crc = (crc>>1)^(0xEDB88320U&-(crc&1));
      table[i] = crc;
      }
    table_computed = 1;
    }

  FILE *F = Fopen(fn, "rb");

  uint8_t buffer[BUFFER_SIZE];
  size_t len;
  uint32_t crc = 0xFFFFFFFF;

  while((len = fread(buffer, 1, BUFFER_SIZE, F)) > 0) 
    for(size_t i = 0 ; i < len ; ++i)
      crc = (crc>>8)^table[(crc^buffer[i])&0xFF];

  fclose(F);

  fprintf(stderr, "CRC32: %08X\n", ~crc);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encode_256(const char *in_path, const char *out_path)
  {
  FILE *FO = Fopen(out_path, "wb");

  size_t in_size = Get_file_size(in_path);

  uint64_t freqs[256] = {0};
  uint32_t cumFreqs[257] = {0};

  Build_frequency_table(in_path, freqs, cumFreqs);

  // WRITE HEADER WITH SIZE AND FREQUENCY TABLE
  fwrite(&in_size, sizeof(uint64_t), 1, FO);
  fwrite(cumFreqs, sizeof(uint32_t), 257, FO);

  Stream *s_out = CreateStream();
  InitStream(s_out, FO, "w");

  FILE *FI = Fopen(in_path, "rb");
  uint8_t buf[BUFFER_SIZE];
  size_t bytesRead;

  // START RANGE CODER
  RangeCoder rc;
  rc_start_encode(&rc);

  // ENCODE INPUT DATA USING THE FREQUENCY TABLE
  while((bytesRead = fread(buf, 1, BUFFER_SIZE, FI)) > 0)
    for(size_t i = 0 ; i < bytesRead ; ++i)
      rc_encode(&rc, 256, buf[i], cumFreqs, s_out);

  // FINISH ENCODE
  rc_finish_encode(&rc, s_out);
  FlushStream(s_out);

  fclose(FO);
  fclose(FI);

  printf("Encoded %"PRIu64" bytes to %"PRIu64" bytes\n",
  in_size, rc.bytes_processed + (sizeof(uint32_t ) * 257));

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Decode_256(const char *in_path, const char *out_path)
  {
  FILE *FI = Fopen(in_path, "rb");
  FILE *FO = Fopen(out_path, "wb");

  uint64_t out_size;
  Fread(&out_size, sizeof(uint64_t), 1, FI);
  fprintf(stderr, "Size: %"PRIu64"\n", out_size);

  uint32_t cumFreqs[257] = {0};
  Fread(cumFreqs, sizeof(uint32_t), 257, FI);
  Print_cum_frequencies(cumFreqs);

  RangeCoder rc;
  Stream *s_in = CreateStream();
  InitStream(s_in, FI, "r");
  rc_start_decode(&rc, s_in);

  // DECODE
  uint8_t buf[BUFFER_SIZE];
  uint8_t *ptr_buf = buf;
  size_t idx = 0;
  size_t decoded = 0;

  while(idx++ < out_size)
    {
    *ptr_buf++ = (uint8_t) rc_decode(&rc, 256, cumFreqs, s_in);
    if(++decoded == BUFFER_SIZE)
      {
      Fwrite(buf, 1, decoded, FO);
      decoded = 0;
      ptr_buf = buf;
      }
    }

  if(decoded != 0)
    Fwrite(buf, 1, decoded, FO);

  fclose(FI);
  fclose(FO);

  printf("Decoded %zu bytes.\n", idx);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintPlot(uint64_t *histogram, int type, int hide_zeros, int max_width)
  {
  // Find the maximum frequency for scaling the histogram
  uint64_t max_value = 0;
  int max_bins = (type == 8 ? MAX_UINT8 : MAX_UINT16);

  for(int i = 0 ; i < max_bins ; ++i)
    if(histogram[i] > max_value)
      max_value = histogram[i];

  // Print the plot
  for(int i = 0 ; i < max_bins ; ++i)
    {
    // Skip empty bins if the hide_zeros flag is set
    if(hide_zeros && histogram[i] == 0)
      continue;

    // Normalize the frequency to fit the histogram within max_width
    int bar_length = (int)((double)histogram[i] / max_value * max_width);

    if(i > 31 && i < 127) // only show printable characters
      printf("%-5d '%c' | ", i, (isprint(i) ? (char)i : '.'));
    else
      printf("%-5d     | ", i); // non-printable characters represented as '.'

    for(int j = 0 ; j < bar_length ; ++j)
      printf("*");
    printf("\n");
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void histogram_mode(int argc, char *argv[])
  {
  int opt;
  int type = 8;
  int plot = 0;
  int hide_zeros = 0;
  int max_width = MAX_HISTOGRAM_PLOT_WIDTH;

  if(argc <= 2)
    {
    print_histogram_usage(argv[0]);
    exit(1);
    }

  optind = 2;
  while((opt = getopt(argc, argv, "ht:w:p")) != -1)
    {
    switch (opt)
      {
      case 'h':
        hide_zeros = 1;
      break;
      case 't':
        type = atoi(optarg);
        if(type != 8 && type != 16)
          {
          fprintf(stderr, "Error: Type must be 8 or 16.\n");
          exit(1);
          }
      break;
      case 'w':
        max_width = atoi(optarg);
      break;
      case 'p':
        plot = 1;
        break;
      default:
        print_histogram_usage(argv[0]);
        exit(1);
      }
    }

  if(optind >= argc)
    {
    fprintf(stderr, "Missing filename for histogram.\n");
    print_histogram_usage(argv[0]);
    exit(1);
    }

  const char *filename = argv[optind];
  FILE *file = Fopen(filename, "rb");
  uint64_t histogram[MAX_UINT16] = {0};
  size_t count = 0;

  if(type == 8)
    {
    uint8_t buffer[BUFFER_SIZE];
    size_t read;

    while((read = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, file)) > 0)
      for(size_t i = 0 ; i < read ; ++i)
        {
        histogram[buffer[i]]++;
        count++;
        }

    if(plot)
      {
      PrintPlot(histogram, 8, hide_zeros, max_width);
      fclose(file);
      return; // Exit after printing plot
      }

    for(int i = 0 ; i < MAX_UINT8 ; ++i)
      if(!hide_zeros || histogram[i] > 0)
        printf("%u, %llu\n", i, (unsigned long long)histogram[i]);
    }
  else
    {
    uint16_t buffer[BUFFER_SIZE];
    size_t read;

    while((read = fread(buffer, sizeof(uint16_t), BUFFER_SIZE, file)) > 0)
      for(size_t i = 0; i < read; ++i)
        {
        histogram[buffer[i]]++;
        count++;
        }

    if(plot)
      {
      PrintPlot(histogram, 16, hide_zeros, max_width);
      fclose(file);
      return; // Exit after printing plot
      }

    for(int i = 0 ; i < MAX_UINT16 ; ++i)
      if(!hide_zeros || histogram[i] > 0)
        printf("%u, %llu\n", i, (unsigned long long)histogram[i]);
    }

  fclose(file);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void distance_mode(int argc, char *argv[])
  {
  int opt;
  char *pattern_str = NULL;
  int show_all_distances = 0;

  if(argc <= 2)
    {
    print_distance_usage(argv[0]);
    exit(1);
    }

  optind = 2;
  while((opt = getopt(argc, argv, "et:")) != -1)
    {
    switch(opt)
      {
      case 'e':
        show_all_distances = 1;
        break;
      case 't':
        pattern_str = optarg;
        break;
      default:
        print_distance_usage(argv[0]);
        exit(1);
      }
    }

  if(!pattern_str || strlen(pattern_str) < 1)
    {
    fprintf(stderr, "Error: Pattern must be a non-empty string.\n");
    print_distance_usage(argv[0]);
    exit(1);
    }

  if(optind >= argc)
    {
    fprintf(stderr, "Missing filename for distance.\n");
    print_distance_usage(argv[0]);
    exit(1);
    }

  const char *filename = argv[optind];
  FILE *file = Fopen(filename, "rb");
  size_t pattern_len = strlen(pattern_str);
  size_t buffer_size = 1024;  // Buffer size for reading chunks
  uint8_t *buffer = Malloc(buffer_size);

  size_t bytes_read;
  size_t pos = 0;
  size_t pattern_pos;
  size_t pattern_last_pos;
  int first = 1;
  size_t n_patterns = 0;
  double sum = 0.0;
  // Read the file in chunks and search for the pattern in the buffer
  while((bytes_read = fread(buffer, 1, buffer_size, file)) > 0)
    {
    for(size_t i = 0 ; i <= bytes_read - pattern_len ; ++i)
      {
      if(memcmp(&buffer[i], pattern_str, pattern_len) == 0)
        {
        pattern_pos = pos + i;
        if(first == 1)
          {
          //printf("Pattern found at position: %zu\n", pattern_pos);
          first = 0;
          n_patterns++;
          pattern_last_pos = pattern_pos;
          }
        else
          {
          //printf("Pattern found at position: %zu\n", pattern_pos);
          n_patterns++;
	  size_t distance = pattern_pos - pattern_last_pos;
          if(show_all_distances)
            printf("Distance: %zu\n", distance);
	  sum += distance;
          pattern_last_pos = pattern_pos;
          }
        }
      }
    pos += bytes_read;
    }

   if(first == 1)
    {
    fprintf(stderr, "Pattern NOT found in the file.\n");
    fclose(file);
    free(buffer);
    exit(1);
    }

  printf("Found %zu patterns\n", n_patterns);

  double average = sum / (n_patterns-1);
  printf("Average Distance: %.2f\n", average);

  fclose(file);
  free(buffer);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void crc_mode(int argc, char *argv[])
  {
  if(argc != 3)
    {
    print_crc_hash_usage(argv[0]);
    exit(1);
    }

  Crc32_from_file(argv[2]);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void xrc_256_mode(int argc, char *argv[])
  {
  if(argc != 5)
    {
    print_xrc_256_usage(argv[0]);
    exit(1);
    }

  if(strcmp(argv[2], "encode") == 0)
    {
    Encode_256(argv[3], argv[4]);  // Compress the file
    }
  else if(strcmp(argv[2], "decode") == 0)
    {
    Decode_256(argv[3], argv[4]);  // Decompress the file
    }
  else
    {
    fprintf(stderr, "Invalid operation. Use 'encode' or 'decode'.\n");
    exit(1);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void pack2_mode(int argc, char *argv[])
  {
  if(argc != 5)
    {
    print_pack2_usage(argv[0]);
    exit(1);
    }

  if(strcmp(argv[2], "pack") == 0)
    {
    Pack_file_2bit(argv[3], argv[4]); 
    }
  else if(strcmp(argv[2], "unpack") == 0)
    {
    Unpack_file_2bit(argv[3], argv[4]); 
    }
  else
    {
    fprintf(stderr, "Invalid operation. Use 'pack' or 'unpack'.\n");
    exit(1);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void pack4_mode(int argc, char *argv[])
  {
  if(argc != 5)
    {
    print_pack4_usage(argv[0]);
    exit(1);
    }

  if(strcmp(argv[2], "pack") == 0)
    {
    Pack_file_4bit(argv[3], argv[4]); 
    }
  else if(strcmp(argv[2], "unpack") == 0)
    {
    Unpack_file_4bit(argv[3], argv[4]); 
    }
  else
    {
    fprintf(stderr, "Invalid operation. Use 'pack' or 'unpack'.\n");
    exit(1);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void entropy_mode(int argc, char *argv[])
  {
  int opt;
  int verbose = 0;

  if(argc <= 2) 
    {
    print_entropy_usage(argv[0]);
    exit(1);
    }

  optind = 2;
  while((opt = getopt(argc, argv, "v")) != -1) 
    {
    switch(opt) 
      {
      case 'v':
        verbose = 1;
      break;
      default:
        print_entropy_usage(argv[0]);
        exit(1);
      }
    }

  if(optind >= argc) 
    {
    fprintf(stderr, "Missing filename for entropy.\n");
    print_entropy_usage(argv[0]);
    exit(1);
    }

  const char *filename = argv[optind];
  FILE *file = Fopen(filename, "rb");

  uint64_t freq[BYTE_RANGE] = {0};
  uint64_t total_bytes = 0;
  uint8_t  buffer[READ_BUFFER_SIZE];

  size_t bytes_read;
  while((bytes_read = fread(buffer, 1, READ_BUFFER_SIZE, file)) > 0) 
    {
    total_bytes += bytes_read;
    for(size_t i = 0 ; i < bytes_read ; i++) 
      freq[buffer[i]]++;
    }

  fclose(file);

  if(total_bytes == 0) 
    {
    fprintf(stderr, "Empty file.\n");
    exit(1);
    }

  double entropy = 0.0;
  for(int i = 0 ; i < BYTE_RANGE ; ++i) 
    {
    if(freq[i] == 0) continue;
    double p = (double)freq[i] / total_bytes;
    entropy -= p * log2(p);
    }

  fprintf(stdout, "Shannon entropy: %.6f bits/byte\n", entropy);

  if(verbose)
    {
    fprintf(stdout, "Total bytes: %"PRIu64"\n", total_bytes);
    for(int i = 0 ; i < BYTE_RANGE ; ++i) 
      if(freq[i] > 0)
        fprintf(stdout, "Byte 0x%02X: %"PRIu64" (%.5f%%)\n", i, freq[i], 
	(freq[i]*100.0) / total_bytes);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, char *argv[])
  {
  if(argc < 2)
    {
    print_main_usage(argv[0]);
    return 1;
    }

  const char *command = argv[1];

  if(strcmp(command, "generate") == 0)
    {
  Generate_sequence(argc, argv);
    }
  else if(strcmp(command, "histogram") == 0)
    {
    histogram_mode(argc, argv);
    }
  else if(strcmp(command, "distance") == 0)
    {
    distance_mode(argc, argv);
    }
  else if(strcmp(command, "crc32-hash") == 0)
    {
    crc_mode(argc, argv);
    }
  else if(strcmp(command, "entropy") == 0)
    {
    entropy_mode(argc, argv);
    }
  else if(strcmp(command, "xrc-256") == 0)
    {
    xrc_256_mode(argc, argv);
    }
  else if(strcmp(command, "pack2") == 0)
    {
    pack2_mode(argc, argv);
    }
  else if(strcmp(command, "pack4") == 0)
    {
    pack4_mode(argc, argv);
    }
  else if(strcmp(command, "profile") == 0)
    {
    Complexity_Profile(argc, argv);
    }
  else if(strcmp(command, "pipelines") == 0)
    {
    print_pipelines();
    }
  else if(strcmp(command, "version") == 0)
    {
    print_version();
    }
  else
    {
    fprintf(stderr, "Unknown command: %s\n", command);
    print_main_usage(argv[0]);
    return 1;
    }

  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
