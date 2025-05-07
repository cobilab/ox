#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define HASH_SIZE 1048576
#define ALPHABET_SIZE 256

typedef struct SymbolCount {
    uint8_t symbol;
    int count;
    struct SymbolCount* next;
} SymbolCount;

typedef struct ContextNode {
    uint8_t* context;
    int total;
    SymbolCount* symbols;
    struct ContextNode* next;
} ContextNode;

ContextNode* hash_table[HASH_SIZE];

// Simple hash function for a k-length context
unsigned int hash_context(uint8_t* context, int k) {
    unsigned int hash = 5381;
    for (int i = 0; i < k; i++)
        hash = ((hash << 5) + hash) + context[i];
    return hash % HASH_SIZE;
}

// Finds or creates a context node in the hash table
ContextNode* find_or_create_context(uint8_t* context, int k) {
    unsigned int h = hash_context(context, k);
    ContextNode* node = hash_table[h];
    while (node) {
        if (memcmp(node->context, context, k) == 0)
            return node;
        node = node->next;
    }

    // Not found, create new
    ContextNode* new_node = (ContextNode*)malloc(sizeof(ContextNode));
    new_node->context = (uint8_t*)malloc(k);
    memcpy(new_node->context, context, k);
    new_node->total = 0;
    new_node->symbols = NULL;
    new_node->next = hash_table[h];
    hash_table[h] = new_node;
    return new_node;
}

// Updates the model with one observation
void update_model(uint8_t* buffer, int len, int k) {
    for (int i = 0; i < len - k; i++) {
        ContextNode* ctx = find_or_create_context(&buffer[i], k);
        uint8_t next_symbol = buffer[i + k];
        SymbolCount* sc = ctx->symbols;
        while (sc) {
            if (sc->symbol == next_symbol) {
                sc->count++;
                break;
            }
            sc = sc->next;
        }
        if (!sc) {
            sc = (SymbolCount*)malloc(sizeof(SymbolCount));
            sc->symbol = next_symbol;
            sc->count = 1;
            sc->next = ctx->symbols;
            ctx->symbols = sc;
        }
        ctx->total++;
    }
}

// ASCII entropy plot function
void plot_entropy_ascii(uint8_t* buffer, int len, int k, double alpha, int window) {
    int width = 200, height = 20;
    double* entropy = (double*)calloc(len, sizeof(double));
    double* smoothed = (double*)calloc(len, sizeof(double));
    if (!entropy || !smoothed) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    double max_entropy = 0.0;
    for (int i = k; i < len; i++) {
        ContextNode* ctx = find_or_create_context(&buffer[i - k], k);
        uint8_t symbol = buffer[i];
        int count = 0;

        for (SymbolCount* sc = ctx->symbols; sc; sc = sc->next) {
            if (sc->symbol == symbol) {
                count = sc->count;
                break;
            }
        }

        double prob = (count + alpha) / (ctx->total + alpha * ALPHABET_SIZE);
        entropy[i] = -log2(prob);
        if (entropy[i] > max_entropy) max_entropy = entropy[i];
    }

    for (int i = k; i < len; i++) {
        double sum = 0;
        int count = 0;
        for (int j = i - window / 2; j <= i + window / 2; j++) {
            if (j >= k && j < len) {
                sum += entropy[j];
                count++;
            }
        }
        smoothed[i] = sum / count;
    }

    char grid[height][width];
    memset(grid, ' ', sizeof(grid));

    int plot_len = len - k;
    for (int x = 0; x < width; x++) {
        int idx = k + (plot_len * x) / width;
        if (idx >= len) continue;
        double val = smoothed[idx];
        int y = (int)((1.0 - val / max_entropy) * (height - 1));
        if (y < 0) y = 0;
        if (y >= height) y = height - 1;
        grid[y][x] = '*';
    }

    printf("\nInformation Content Profile (k=%d, α=%.2f):\n\n", k, alpha);
    for (int y = 0; y < height; y++) {
        printf("%2d |", height - y - 1);
        for (int x = 0; x < width; x++)
            putchar(grid[y][x]);
        putchar('\n');
    }
    printf("    ");
    for (int i = 0; i < width + 1; i++) putchar('-');
    printf("\n     Start →\n\n");

    free(entropy);
    free(smoothed);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <filename> <k> <alpha>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    int k = atoi(argv[2]);
    double alpha = atof(argv[3]);

    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Failed to open file");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* buffer = (uint8_t*)malloc(fsize);
    if (!buffer) {
        perror("Failed to allocate memory");
        fclose(f);
        return 1;
    }

    fread(buffer, 1, fsize, f);
    fclose(f);

    update_model(buffer, fsize, k);
    plot_entropy_ascii(buffer, fsize, k, alpha, 1);

    free(buffer);
    return 0;
}

