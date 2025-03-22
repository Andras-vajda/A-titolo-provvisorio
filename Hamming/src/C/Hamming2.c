#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_N 32  // Numero massimo di elementi nell'insieme
#define LUT_THRESHOLD 16 // Limite oltre il quale non usiamo la LUT
#define MAX_D 8    // Massima distanza di Hamming supportata

// LUT per le combinazioni a distanza fissa
typedef struct {
    uint32_t subsets[1 << MAX_D]; // Array di subset a distanza D
    size_t count;                 // Numero di subset validi
} LUT_Hamming;

LUT_Hamming hammingLUT[LUT_THRESHOLD + 1][MAX_D + 1];

// Funzione per generare i sottoinsiemi a distanza Hamming fissa (solo per n <= 16)
void genera_LUT_Hamming(size_t n) {
    if (n > LUT_THRESHOLD) return; // Evitiamo LUT per n > 16
    for (size_t d = 1; d <= MAX_D; ++d) {
        size_t index = 0;
        for (uint32_t dm = (1 << d) - 1, limite = 1 << n; dm < limite;
             dm = (dm + 1) | ((dm & -dm) - 1)) {
            hammingLUT[n][d].subsets[index++] = dm;
        }
        hammingLUT[n][d].count = index;
    }
}

// Generazione on-the-fly per n > 16
typedef uint32_t Set_t;
void genera_hamming_onthefly(size_t n, size_t d) {
    printf("[Generazione on-the-fly per n=%zu, d=%zu]\n", n, d);
    size_t count = 0;
    const Set_t limite = 1U << n; // Precalcolo del valore limite
    for (Set_t dm = (1 << d) - 1; dm < limite;
         dm = (dm + 1) | ((dm & -dm) - 1)) {
        count++;
    }
    printf("Sottoinsiemi generati: %zu\n", count);
}

// Benchmark comparativo
void benchmark(size_t n, size_t d) {
    clock_t start, end;
    double time_taken;

    if (n <= LUT_THRESHOLD) {
        start = clock();
        genera_LUT_Hamming(n);
        end = clock();
        time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("[LUT] Tempo per n=%zu, d=%zu: %f sec\n", n, d, time_taken);
    }

    start = clock();
    genera_hamming_onthefly(n, d);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("[On-the-fly] Tempo per n=%zu, d=%zu: %f sec\n", n, d, time_taken);
}

// Funzione principale
int main() {
    size_t n = 16; // Test con il massimo valore gestito dalla LUT
    size_t d = 3;
    benchmark(n, d);
    
    n = 22; // Test per generazione on-the-fly
    d = 4;
    benchmark(n, d);
    
    return 0;
}
