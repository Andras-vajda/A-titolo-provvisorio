/**********************************************************************/
/*                                                                    */
/* Hamming_DL.c (Refactored for Visual Studio)                        */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definizione del tipo per rappresentare l'insieme
typedef uint32_t Set_t;

// Pensiamo in grande, ma non troppo.
// 32 bit saranno sufficienti, per ora!
#define MAX_N 32

// Totalizzatore insiemi generati
size_t counter = 0;

// Una comoda macro per la stampa dei subset
#ifdef _MSC_VER
// Usa BMI1 se supportato
#include <intrin.h>
#if defined(__AVX2__) || defined(__BMI1__) 
#pragma intrinsic(__lzcnt)
#define HAS_MORE_BITS(value, i) ((value) >> (31 - __lzcnt(value)) > (1U << (i)))
#pragma message ("MSVC - posso usare intrinsic BMI1")
#else
// Fallback su una ordinaria formula bitfiddling
#define HAS_MORE_BITS(value, i) ((value) & ~((1U << ((i) + 1)) - 1))
#pragma message ("MSVC - uso di macro bitfiddling")
#endif
#else
#error Questo codice è progettato unicamente per Visual Studio.
#endif

// Funzione per stampare un sottoinsieme
void print_set(Set_t value, size_t n) {
    printf(" {");
    for (char i = 0; i < n; ++i) {
        if (value & (1 << i)) {
            printf("%c", 'a' + i); // Converte la posizione in carattere (a, b, c, ...)
            if (i < n - 1 && HAS_MORE_BITS(value, i)) {
                printf(", "); // Aggiunge la virgola solo se ci sono altri elementi
            }
        }
    }
    printf("}");
}

// Stampa il vettore caratteristico in hex e binario 
void print_binary(Set_t value, size_t n, size_t d, bool prefix) {
    Set_t v = value;
    // Informazioni aggiuntive sul subset nella classe di distanza
    if (prefix) {
        printf("%4zu [%zu]: (%06X) ", counter, d, value);
    }
    
    // Stampa il vettore booleano caratteristico
    for (size_t i = n; i > 0; --i) {
        printf("%c", (value & (1 << (i - 1))) ? '1' : '0');
    }
    
    // Arricchisce l'output con una stampa del subset in formato simbolico
    print_set(v, n);
    puts("");
}

// Genera il successore del subset corrente
Set_t next_subset(Set_t subset, Set_t limit) {
    if (subset == 0 || subset >= limit)
        return limit;
    Set_t smallest = subset & -subset;
    Set_t ripple = subset + smallest;
    Set_t ones = subset ^ ripple;
    ones = (ones >> 2) / smallest;
    return ripple | ones;
}

// Funzione per generare k-subset a distanza desiderata
void generate_k_subset(size_t n, size_t k, size_t d) {
    Set_t base_set = (1 << k) - 1;
    Set_t diff_set = (1 << (n - k)) - 1;
    Set_t base_mask = (1 << d) - 1;

    if (d == 0) {
        counter = 0;
        printf("** Base set: ");
        print_binary(base_set, n, d, false);
        return;
    }

    if (d > k) {
        printf("Distanza non valida: d deve essere <= k.\n");
        return;
    }

    while (base_mask < (1 << k)) {
        Set_t diff_mask = (1 << d) - 1;
        while (diff_mask < (1 << (n - k))) {
            Set_t modified_base = base_set & ~base_mask;
            Set_t shifted_diff = diff_mask << k;
            Set_t result = modified_base | shifted_diff;
            ++counter;
            print_binary(result, n, d, true);
            diff_mask = next_subset(diff_mask, (1 << (n - k)));
        }
        base_mask = next_subset(base_mask, (1 << k));
    }
}

// Generazione esaustiva di tutti i k-subset con 1 <= d <= k
void generate_all_subsets(size_t n, size_t k) {
    for (size_t d = 0; d <= k; ++d) {
        printf("\n=== d = %zu ===\n", d);
        generate_k_subset(n, k, d);
    }
}

int main(int argc, char *argv[]) {
    size_t n, k, d;
    
    if (argc != 3) {
        fprintf(stderr, "** Uso: %s n k\n"
                "** k > 2 e 2k <= n <= %d.\n", 
                argv[0], MAX_N);
        return EXIT_FAILURE;
    } 
        
    n = (size_t)atoi(argv[1]);
    k = (size_t)atoi(argv[2]);
    
    srand((unsigned int)time(NULL));
    
    if (k < 2 || n < 2 * k || n > MAX_N) {
        fprintf(stderr, "Errore: Assicurati che 2 < k e 2k <= n <= %d.\n", MAX_N);
        return 1;
    }
    
    d = (rand() % k) + 1;
    printf("** FASE 1: Generazione dei %zu-subset a distanza casuale (%zu)\n", k, d);
    generate_k_subset(n, k, 0);
    generate_k_subset(n, k, d);

    printf("\n** FASE 2: Generazione esaustiva dei %zu-subset\n"
           "**         ordinati per distanza di Hamming crescente:\n", k);
    generate_all_subsets(n, k);
    printf("\n** Subset generati: %zu\n\n", counter);

    return EXIT_SUCCESS;
}
/** EOF **/
