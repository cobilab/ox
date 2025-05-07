#ifndef MSG_H
#define MSG_H

#include <stdio.h>
#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_pipelines(void)
  {
  fprintf(stderr,
  "1) Compression of a DNA sequence:                                \n"
  "                                                                 \n"
  "#!/bin/bash                                                      \n"
  "grep -v '>' DNA.fa | tr -d -c 'ACGT' | tr 'ACGT' 'ABCD' > A.seq  \n"
  "#                                                                \n"
  "./ox pack2 pack A.seq A.packed                                   \n"
  "./ox xrc-256 encode A.packed A.encoded                           \n"
  "#                                                                \n"
  "./ox xrc-256 decode A.encoded A.decoded                          \n"
  "./ox pack2 unpack A.decoded A.unpacked                           \n"
  "#                                                                \n"
  "cmp A.unpacked A.seq                                             \n"
  "                                                                 \n");

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_main_usage(const char *progname)
  {
  printf("   ____ ____ \n"
         "  ||o |||x ||\n"
         "  ||__|||__||\n"
         "  |/__\\|/__\\|\n\n");
  printf("Usage:\n");
  printf("  %s generate [-s <size>] [-c <cardinality>] [-e <seed>] <filename>\n",
  progname);
  printf("  %s histogram [-h] [-t 8|16] [-w <width>] [-p] <filename>\n",
  progname);
  printf("  %s distance -t <pattern> <filename>\n", progname);
  printf("  %s crc32-hash <filename>\n", progname);
  printf("  %s entropy [-v] <filename>\n", progname);
  printf("  %s xrc-256 [encode|decode] <filename> <filename>\n", progname);
  printf("  %s pack2 [pack|unpack] <filename> <filename>\n", progname);
  printf("  %s pack4 [pack|unpack] <filename> <filename>\n", progname);
  printf("  %s profile [-k <ctx>] [-a <alphaDen>] [-w <width>] <filename>\n",
  progname);

  printf("  %s pipelines\n", progname);
  printf("  %s version\n", progname);
  printf("\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_version(void)
  {
  printf("v%d.%d\n", VERSION, RELEASE);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_generate_usage(const char *progname)
  {
  printf("Usage: %s generate [-s <size>] [-c <cardinality>] [-e <seed>] "
  "<filename>\n", progname);
  printf("  -s <size>         Size of the generated sequence\n");
  printf("  -c <cardinality>  Alphabet cardinality (0 to 255)\n");
  printf("  -e <seed>         Seed for random number generation\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_histogram_usage(const char *progname)
  {
  printf("Usage: %s histogram [-h] [-t 8|16] [-w <width>] <filename>\n",
  progname);
  printf("  -h          Hide zero-count entries\n");
  printf("  -t 8|16     Data type (8-bit or 16-bit values)\n");
  printf("  -w <width>  Maximum width to build plot\n");
  printf("  -p          Print a plot instead of raw data\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_distance_usage(const char *progname)
  {
  printf("Usage: %s distance [-t <pattern>] <filename>\n", progname);
  printf("  -t <pattern>   String pattern to search for (e.g., RRR, EXFGGHH)\n");
  printf("  -e             Show individual distances between singular values\n");
  printf("                 Distance: <pattern>xxx<pattern> = 3 + pattern length\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_crc_hash_usage(const char *progname)
  {
  fprintf(stderr, "Usage: %s crc32-hash <filename>\n", progname);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_entropy_usage(const char *progname)
  {
  fprintf(stderr, "Usage: %s entropy [-v] <filename>\n", progname);
  fprintf(stderr, "  -v    Verbose output\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_xrc_256_usage(const char *progname)
  {
  printf("Usage: %s xrc-256 [encode|decode] <filename> <filename>\n", progname);
  fprintf(stderr, "  Encode: %s xrc-256 encode <input> <output>\n", progname);
  fprintf(stderr, "  Decode: %s xrc-256 decode <input> <output>\n", progname);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_pack2_usage(const char *progname)
  {
  printf("Usage: %s pack2 [encode|decode] <filename> <filename>\n", progname);
  fprintf(stderr, "  Pack: %s pack2 pack <input> <output>\n", progname);
  fprintf(stderr, "  Unpack: %s pack2 unpack <input> <output>\n", progname);
  fprintf(stderr, "  It assumes that sequence is composed by 'ABCD'\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_pack4_usage(const char *progname)
  {
  printf("Usage: %s pack4 [encode|decode] <filename> <filename>\n", progname);
  fprintf(stderr, "  Pack: %s pack4 pack <input> <output>\n", progname);
  fprintf(stderr, "  Unpack: %s pack4 unpack <input> <output>\n", progname);
  fprintf(stderr, "  It assumes that sequence is composed by 'AB...OP'\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void print_profile_usage(const char *progname)
  {
  printf("Usage: %s profile [-k <ctx>] [-a <alphaDen>] [-w <window>]"
  "<filename>\n", progname);
  printf("  -k <ctc>      Finite-context model context order\n");
  printf("  -a <alphaDen> Alpha denomiator (1/a) for probability estimation\n");
  printf("  -w <window>   Window size for creating the complexity profile\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

