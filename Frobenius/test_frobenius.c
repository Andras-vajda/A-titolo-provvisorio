/**
 * test_frobenius.c
 * ==============================================================
 * Test suite per l'implementazione del problema di Frobenius in C
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
 * Struttura per un caso di test
 */
typedef struct {
    FrobUint32_t coins[10];  /* Array delle monete per il test */
    FrobUint32_t num_coins;  /* Numero di monete */
    FrobInt32_t expected;    /* Risultato atteso */
    const char* description; /* Descrizione del test */
} TestCase;

/* Esegue tutti i test e restituisce il numero di fallimenti */
int run_tests(void) {
    int failures = 0;
    
    /* Definisci i casi di test */
    TestCase test_cases[] = {
        /* Casi con due monete - formula chiusa */
        {{3, 5}, 2, 7, "Caso base n=2: [3,5]"},
        {{7, 11}, 2, 60, "Caso n=2 valori medi: [7,11]"},
        {{17, 19}, 2, 305, "Caso n=2 valori grandi: [17,19]"},
        
        /* Casi con tre monete - Round-Robin */
        {{3, 5, 7}, 3, 4, "Caso base n=3: [3,5,7]"},
        {{6, 9, 20}, 3, 43, "Problema McNugget: [6,9,20]"},
        {{11, 13, 15}, 3, 37, "Caso tre monete medie: [11,13,15]"}
    };
    
    int num_tests = sizeof(test_cases) / sizeof(TestCase);
    printf("Esecuzione di %d test...\n", num_tests);
    
    /* Esegui ogni test */
    for (int i = 0; i < num_tests; i++) {
        TestCase test = test_cases[i];
        FrobInt32_t result;
        
        /* Scegli l'algoritmo appropriato in base al numero di monete */
        if (test.num_coins == 2) {
            result = frobenius_n2(test.coins[0], test.coins[1]);
        } else {
            result = frobenius_round_robin(test.coins, test.num_coins);
        }
        
        /* Verifica il risultato */
        if (result == test.expected) {
            printf("Test %d: %s - OK (risultato: %d)\n", i+1, test.description, result);
        } else {
            printf("Test %d: %s - FALLITO (atteso: %d, ottenuto: %d)\n", 
                   i+1, test.description, test.expected, result);
            failures++;
        }
    }
    
    /* Stampa il riepilogo */
    printf("\nRiepilogo: %d test, %d successi, %d fallimenti.\n", 
           num_tests, num_tests - failures, failures);
    
    return failures;
}

/**
 * Funzione principale
 */
int main(void) {
    printf("================================\n");
    printf("Test suite per Frobenius.c\n");
    printf("================================\n\n");
    
    int failures = run_tests();
    
    return (failures == 0) ? 0 : 1;  /* Ritorna 0 se tutti i test passano */
}