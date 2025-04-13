/**
 * Frobenius_simple.c
 * ==============================================================
 * Implementazione minimale del problema di Frobenius in C
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

/* Definizioni di tipi standard */
typedef int32_t FrobInt32_t;
typedef uint32_t FrobUint32_t;
typedef uint64_t FrobUint64_t;

/**
 * Calcola il MCD di due valori
 */
FrobUint32_t gcd(FrobUint32_t a, FrobUint32_t b) {
    FrobUint32_t temp;
    while (b != 0U) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/**
 * Risolve il problema di Frobenius per n=2 con formula chiusa
 * g(a,b) = a*b - a - b, se gcd(a,b) = 1
 */
FrobInt32_t frobenius_n2(FrobUint32_t a, FrobUint32_t b) {
    FrobUint32_t temp;
    
    /* Assicura a <= b */
    if (a > b) {
        temp = a;
        a = b;
        b = temp;
    }
    
    /* Verifica che MCD = 1 */
    if (gcd(a, b) != 1U) {
        fprintf(stderr, "Errore: Le monete devono essere coprimi (MCD=1)\n");
        return -1;
    }
    
    /* Formula chiusa: a*b - a - b */
    return (FrobInt32_t)((a * b) - a - b);
}

/**
 * Implementazione semplificata dell'algoritmo Round-Robin per n>2
 */
FrobInt32_t frobenius_round_robin(FrobUint32_t coins[], FrobUint32_t num_coins) {
    FrobUint32_t i;
    FrobUint32_t j;
    FrobUint32_t c;
    FrobUint32_t remainder;
    FrobUint32_t a;
    FrobUint32_t iterations = 0U;
    bool modified = true;
    FrobUint32_t max_val;
    
    /* Verifica input valido */
    if (coins == NULL || num_coins < 3U) {
        fprintf(stderr, "Errore: Serve un array di almeno 3 monete\n");
        return -1;
    }
    
    /* Trova la moneta piu' piccola (a) */
    a = coins[0];
    for (i = 1U; i < num_coins; i++) {
        if (coins[i] < a) {
            a = coins[i];
        }
    }
    
    /* Alloca e inizializza array degli stati */
    FrobUint32_t* states = (FrobUint32_t*)malloc(a * sizeof(FrobUint32_t));
    if (states == NULL) {
        fprintf(stderr, "Errore: Fallita allocazione memoria\n");
        return -1;
    }
    
    /* Inizializza gli stati con "infinito" (valore massimo) */
    for (i = 0U; i < a; i++) {
        states[i] = UINT32_MAX;
    }
    states[0] = 0U; /* La classe residuo 0 parte da 0 */
    
    /* Algoritmo principale Round-Robin */
    while (modified && iterations < 1000U) {
        modified = false;
        iterations++;
        
        /* Crea un buffer temporaneo per i nuovi stati */
        FrobUint32_t* new_states = (FrobUint32_t*)malloc(a * sizeof(FrobUint32_t));
        if (new_states == NULL) {
            free(states);
            fprintf(stderr, "Errore: Fallita allocazione memoria\n");
            return -1;
        }
        
        /* Copia gli stati attuali nel buffer */
        for (i = 0U; i < a; i++) {
            new_states[i] = states[i];
        }
        
        /* Aggiorna gli stati con ogni moneta (esclusa la piu' piccola) */
        for (c = 0U; c < num_coins; c++) {
            FrobUint32_t coin = coins[c];
            
            /* Salta la moneta piu' piccola (a) */
            if (coin == a) {
                continue;
            }
            
            for (i = 0U; i < a; i++) {
                /* Salta stati non inizializzati */
                if (states[i] == UINT32_MAX) {
                    continue;
                }
                
                /* Calcola il nuovo resto e valore */
                remainder = (i + coin) % a;
                FrobUint32_t new_val = states[i] + coin;
                
                /* Se il resto e' minore dell'indice, abbiamo fatto il "giro" */
                if (remainder < i) {
                    new_val -= a;
                }
                
                /* Aggiorna lo stato se migliore */
                if (new_val < new_states[remainder]) {
                    new_states[remainder] = new_val;
                    modified = true;
                }
            }
        }
        
        /* Aggiorna gli stati con i nuovi valori */
        for (i = 0U; i < a; i++) {
            states[i] = new_states[i];
        }
        
        free(new_states);
    }
    
    /* Trova il valore massimo tra gli stati */
    max_val = 0U;
    for (i = 0U; i < a; i++) {
        if (states[i] != UINT32_MAX && states[i] > max_val) {
            max_val = states[i];
        }
    }
    
    /* Libera la memoria e restituisci il risultato */
    free(states);
    
    /* Il numero di Frobenius e' il massimo valore meno a */
    return (FrobInt32_t)(max_val - a);
}

/**
 * Test dei casi noti
 */
int main(void) {
    /* Test caso n=2 */
    FrobUint32_t a = 3U;
    FrobUint32_t b = 5U;
    FrobInt32_t result = frobenius_n2(a, b);
    printf("Caso n=2: g(%u,%u) = %d\n", a, b, result);
    
    /* Test caso n=3 con Round-Robin */
    FrobUint32_t coins3[] = {3U, 5U, 7U};
    result = frobenius_round_robin(coins3, 3U);
    printf("Caso n=3: g(3,5,7) = %d\n", result);
    
    /* Test caso McNugget */
    FrobUint32_t mcnugget[] = {6U, 9U, 20U};
    result = frobenius_round_robin(mcnugget, 3U);
    printf("McNugget number: g(6,9,20) = %d\n", result);
    
    return EXIT_SUCCESS;
}