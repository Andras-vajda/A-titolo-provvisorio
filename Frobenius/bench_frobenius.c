/**
 * bench_frobenius.c
 * ==============================================================
 * Suite di benchmark per l'implementazione del problema di Frobenius in C
 * ==============================================================
 * 
 * @author: M.A.W. 1968
 * @version: 1.0.0
 * @date: 2025-04-13
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/* Definizioni di tipi standard */
typedef int32_t FrobInt32_t;
typedef uint32_t FrobUint32_t;
typedef uint64_t FrobUint64_t;

/* Prototipi delle funzioni da testare (duplicate dal file principale) */
FrobUint32_t gcd(FrobUint32_t a, FrobUint32_t b);
FrobInt32_t frobenius_n2(FrobUint32_t a, FrobUint32_t b);
FrobInt32_t frobenius_round_robin(FrobUint32_t coins[], FrobUint32_t num_coins);

/* Implementazione delle funzioni da testare */
#include "../Frobenius.c"

/**
 * Struttura per un benchmark
 */
typedef struct {
    FrobUint32_t coins[10];  /* Array delle monete per il benchmark */
    FrobUint32_t num_coins;  /* Numero di monete */
    const char* description; /* Descrizione del benchmark */
    int iterations;          /* Numero di iterazioni per un timing preciso */
} BenchmarkCase;

/**
 * Esegue il benchmark specificato e restituisce il tempo in millisecondi
 */
double benchmark_case(const BenchmarkCase* bench) {
    clock_t start, end;
    double cpu_time_used;
    FrobInt32_t result = 0;
    
    printf("Benchmark: %s - ", bench->description);
    fflush(stdout);  /* Flush dell'output prima di misurare il tempo */
    
    start = clock();
    
    /* Esegui il benchmark più volte per avere un timing preciso */
    for (int i = 0; i < bench->iterations; i++) {
        if (bench->num_coins == 2) {
            result = frobenius_n2(bench->coins[0], bench->coins[1]);
        } else {
            result = frobenius_round_robin(bench->coins, bench->num_coins);
        }
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000.0 / bench->iterations;
    
    printf("Risultato: %d, Tempo: %.3f ms\n", result, cpu_time_used);
    
    return cpu_time_used;
}

/**
 * Funzione principale
 */
int main(void) {
    printf("================================\n");
    printf("Benchmark suite per Frobenius.c\n");
    printf("================================\n\n");
    
    /* Definisci i casi di benchmark */
    BenchmarkCase benchmark_cases[] = {
        /* Casi con due monete - formula chiusa */
        {{3, 5}, 2, "Caso base n=2: [3,5]", 10000},
        {{7, 11}, 2, "Caso n=2 valori medi: [7,11]", 10000},
        {{17, 19}, 2, "Caso n=2 valori grandi: [17,19]", 10000},
        
        /* Casi con tre monete - Round-Robin */
        {{3, 5, 7}, 3, "Caso base n=3: [3,5,7]", 1000},
        {{6, 9, 20}, 3, "Problema McNugget: [6,9,20]", 1000},
        {{11, 13, 15}, 3, "Caso tre monete medie: [11,13,15]", 100}
    };
    
    int num_benchmarks = sizeof(benchmark_cases) / sizeof(BenchmarkCase);
    
    printf("Esecuzione di %d benchmark...\n\n", num_benchmarks);
    
    /* Esegui ogni benchmark */
    double total_time = 0.0;
    for (int i = 0; i < num_benchmarks; i++) {
        double time = benchmark_case(&benchmark_cases[i]);
        total_time += time;
    }
    
    /* Stampa il riepilogo */
    printf("\nRiepilogo: %d benchmark completati, tempo totale medio: %.3f ms\n", 
           num_benchmarks, total_time / num_benchmarks);
    
    return 0;
}