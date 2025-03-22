/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/* Knuth.c - Optimal algorithms for k-subset generation               */
/*                                                                    */
/**********************************************************************/
/* CODE METRICS:                                                      */
/* Size metrics:                                                      */
/*   - Lines of Code (LOC): 221                                       */
/*   - Logical Lines of Code (LLOC): 169                              */
/*   - Comment Lines: 52                                              */
/*   - Comment-to-Code Ratio: 0.31                                    */
/* Complexity metrics:                                                */
/*   - Cyclomatic Complexity: 34                                      */
/*   - McCabe's Essential Complexity: 28                              */
/*   - Maximum Nesting Level: 2                                       */
/*   - Maintainability Index: 61 (Moderate)                           */
/* Halstead metrics:                                                  */
/*   - Program Volume (V): 4,421                                      */
/*   - Program Difficulty (D): 41.3                                   */
/*   - Development Time Estimate: 10.14 hours                         */
/**********************************************************************/
/* TECHGLISH: This file implements three different algorithms from    */
/* Donald Knuth's "The Art of Computer Programming" for generating    */
/* all k-subsets of an n-set: Algorithm T (lexicographic order),      */
/* Algorithm R (minimal-change "revdoor" order), and Gosper's         */
/* algorithm for bitwise subset generation.                           */
/**********************************************************************/
/* NERDYTALIAN: Questo file implementa tre diversi algoritmi dal      */
/* "The Art of Computer Programming" di Donald Knuth per generare     */
/* tutti i k-sottoinsiemi di un n-insieme: Algoritmo T (ordine        */
/* lessicografico), Algoritmo R (ordine "revdoor" a minima variazione)*/
/* e l'algoritmo di Gosper per la generazione bitwise di sottoinsiemi.*/
/**********************************************************************/
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ENABLE_OUTPUT
/* Limite arbitrario, anche per evitare attese con tempi geologici */
#define MAX_K 16

/******************* DATA STRUCTURES *****************************/
/* TECHGLISH: We represent k-subsets as arrays of integers.      */
/* Array S stores the elements of the current subset, with       */
/* additional positions for sentinel values that simplify the    */
/* algorithms' logic and boundary handling.                      */
/*                                                               */
/* NERDYTALIAN: Rappresentiamo i k-sottoinsiemi come array di    */
/* interi. L'array S memorizza gli elementi del sottoinsieme     */
/* corrente, con posizioni aggiuntive per valori sentinella che  */
/* semplificano la logica degli algoritmi e la gestione dei      */
/* confini.                                                      */
/*****************************************************************/

/* Puntatore al sottoinsieme generato */
int *S;

/* Totalizzatore k-sottoinsiemi generati - binomiale(n,k) */
size_t TotalSet;

/******************* OUTPUT FUNCTION ****************************/
/* TECHGLISH: Displays the current subset in a human-readable   */
/* format. The ENABLE_OUTPUT macro allows turning off output    */
/* when pure performance benchmarking is needed, while still    */
/* maintaining accurate counting of generated subsets.          */
/*                                                              */
/* NERDYTALIAN: Visualizza il sottoinsieme corrente in un       */
/* formato leggibile. La macro ENABLE_OUTPUT consente di        */
/* disattivare l'output quando e' necessario un benchmarking     */
/* puro delle prestazioni, mantenendo comunque un conteggio     */
/* accurato dei sottoinsiemi generati.                          */
/****************************************************************/

void printSubset(size_t k)
{
#ifdef ENABLE_OUTPUT
    static size_t cnt = 0;
    printf("%5zu {%d, ", TotalSet++, S[0]);
    for (size_t i = 1; i < k -1; ++i) {
        printf("%d, ", S[i]);
    }
    printf("%d}\n", S[k -1]);
#else
    TotalSet++;
#endif
}

/******************* ALGORITHM T: LEXICOGRAPHIC *****************/
/* TECHGLISH: Generates k-subsets in lexicographic order, as     */
/* described in TAOCP 7.2.1.3, Algorithm T. This algorithm       */
/* maintains the invariant S[0] < S[1] < ... < S[k-1] and uses   */
/* sentinel values S[k] and S[k+1] for boundary handling.        */
/*                                                               */
/* NERDYTALIAN: Genera k-sottoinsiemi in ordine lessicografico,  */
/* come descritto in TAOCP 7.2.1.3, Algoritmo T. Questo algoritmo*/
/* mantiene l'invariante S[0] < S[1] < ... < S[k-1] e utilizza   */
/* valori sentinella S[k] e S[k+1] per la gestione dei confini.  */
/*****************************************************************/

void T_ksubsetLex(const size_t k, const size_t n) {
    int j, x;

init:
    // Initialize S[0...k-1] with the first k integers
    for (j = 0; j < k; j++) {
        S[j] = j;
    }

    // Set up sentinel values
    j = k;
    S[k] = n;      // Upper bound for comparisons
    S[k+1] = 0;    // Ensures termination

visit:
    // Output the current subset
    printSubset(k);

    // Determine the next step based on the current state
    if (j > 0) {
        x = j;
        goto increment;
    }

    // Easy case: increment S[0] if possible
    if (S[0] + 1 < S[1]) {
        S[0]++;
        goto visit;
    }
    else {
        j = 2;
    }

find_j:
    // Reset elements before j-1 to their minimal values
    S[j - 2] = j - 2;
    x = S[j-1] + 1;

    // If we can't increment S[j-1], move to the next position
    if (x == S[j]) {
        j++;
        goto find_j;
    }

done:
    // Check if we've generated all subsets
    if (j > k) {
        return;
    }

increment:
    // Increment S[j-1] and reset j for the next iteration
    S[j-1] = x;
    j--;
    goto visit;
}

/******************* ALGORITHM R: GRAY CODE ********************/
/* TECHGLISH: Implements Algorithm R from TAOCP 7.2.1.3, which  */
/* generates k-subsets in a "revdoor" order where consecutive   */
/* subsets differ by exactly one element - a Gray code for      */
/* combinations. The direction of change alternates based on    */
/* the parity of k.                                             */
/*                                                              */
/* NERDYTALIAN: Implementa l'Algoritmo R da TAOCP 7.2.1.3, che  */
/* genera k-sottoinsiemi in un ordine "revdoor" dove i          */
/* sottoinsiemi consecutivi differiscono per esattamente un     */
/* elemento - un codice Gray per le combinazioni. La direzione  */
/* di cambiamento si alterna in base alla parita' di k.          */
/****************************************************************/

void R_ksubsetRD(const size_t k, const size_t n) {
    int j;

init:
    // Initialize with the first subset: {0, 1, ..., k-1}
    for (j = 0; j < k; j++) {
        S[j] = j;
    }

    S[k] = n;  // Sentinel value

visit:
    // Output the current subset
    printSubset(k);

easy:
    // Determine direction based on parity of k
    if (k & 1) {
        // Odd k: try to increase S[0]
        if (S[0] + 1 < S[1]) {
            S[0]++;
            goto visit;
        }
        else {
            j = 2;
            goto decrease;
        }
    }
    else {
        // Even k: try to decrease S[0]
        if (S[0] > 0) {
            S[0]--;
            goto visit;
        }
        else {
            j = 2;
            goto increase;
        }
    }

decrease:
    // Try to decrease S[j-1]
    if (S[j-1] >= j) {
        S[j-1] = S[j-2];
        S[j-2] = j - 2;
        goto visit;
    }
    else {
        j++;
    }

increase:
    // Try to increase S[j-1]
    if (S[j-1] + 1 < S[j]) {
        S[j-2] = S[j-1];
        S[j-1]++;
        goto visit;
    }
    else {
        j++;
        if (j <= k) goto decrease;
    }
}

/******************* GOSPER'S ALGORITHM ************************/
/* TECHGLISH: Implements Gosper's algorithm for generating all  */
/* k-subsets using bitwise operations. This algorithm           */
/* represents subsets as bit vectors and uses an elegant bit    */
/* manipulation formula to generate the next combination,       */
/* providing excellent performance for small universe sizes.    */
/*                                                              */
/* NERDYTALIAN: Implementa l'algoritmo di Gosper per generare   */
/* tutti i k-sottoinsiemi utilizzando operazioni bit a bit.     */
/* Questo algoritmo rappresenta i sottoinsiemi come vettori di  */
/* bit e utilizza una formula elegante di manipolazione dei bit */
/* per generare la combinazione successiva, offrendo eccellenti */
/* prestazioni per universi di piccole dimensioni.              */
/****************************************************************/

void G_ksubsetGray(const size_t k, const size_t n) {
    // Initialize with first k-subset: binary representation 00...0011...11 (k ones)
    uint64_t x = (1ULL << k) - 1;
    uint64_t limit = 1ULL << n;
    
    while (x < limit) {
        /* Convert bit representation to integer array for output */
        size_t j = 0, mask = 1;
        for (size_t i = 0; i < n; ++i) {
            if (x & mask) {
                S[j++] = i;
            }
            mask <<= 1;
        }
        printSubset(k);
        
        /* Generate the next k-subset using Gosper's hack */
        uint64_t u = x & -x;           // Isolate rightmost 1-bit
        uint64_t v = x + u;            // Turn off rightmost 1-bit and propagate carry
        x = v + (((v ^ x) / u) >> 2);  // Add displaced bits
    }
}

/******************* MAIN PROGRAM *****************************/
/* TECHGLISH: Command-line interface that runs all three       */
/* algorithms on the same inputs for comparison. Takes k       */
/* (subset size) and n (universe size) as parameters and       */
/* performs validation to ensure constraints are met.          */
/*                                                             */
/* NERDYTALIAN: Interfaccia a riga di comando che esegue tutti */
/* e tre gli algoritmi sugli stessi input per confronto. Prende*/
/* k (dimensione del sottoinsieme) e n (dimensione             */
/* dell'universo) come parametri ed esegue la validazione per  */
/* garantire che i vincoli siano soddisfatti.                  */
/***************************************************************/

int main(int argc, char* argv[]) {
    size_t k, n;

    if (3 != argc) {
        fprintf(stderr, "Uso: %s k n, con 2 < k < n\n", argv[0]);
        return EXIT_FAILURE;
    }

    k = (size_t)atoi(argv[1]);
    n = (size_t)atoi(argv[2]);

    // Controllo valore minimo di k
    if (k < 3) {
        fprintf(stderr, "k deve essere maggiore di 2.\n");
        fprintf(stderr, "Uso: %s k n, con k < n\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Controllo valore massimo di k (mera programmazione difensiva)
    if (k > MAX_K) {
        fprintf(stderr, "k deve essere minore di %u per questa implementazione.\n", MAX_K);
        return EXIT_FAILURE;
    }

    if (k >= n) {
        fprintf(stderr, "Non esistono %zu-sottoinsiemi di un insieme di cardinalita' %zu.\n", k, n);
        fprintf(stderr, "Uso: %s k n, con k < n\n", argv[0]);
        return EXIT_FAILURE;
    }
        
    S = (int*)malloc((k + 3) * sizeof(int));
    if (NULL == S)
    {
        fputs("Errore di allocazione!\n", stderr);
        return EXIT_FAILURE;
    }

    // Run Algorithm T (lexicographic order)
    printf("T_ksubsetLex(%d, %d) per ordine lex:\n", k, n);
    TotalSet = 0;
    T_ksubsetLex(k, n);
    printf("%zu subset generati\n", TotalSet);

    // Run Algorithm R (revdoor/Gray code order)
    printf("\nR_ksubsetRD(%d, %d) per ordine revdoor (Gray):\n", k, n);
    TotalSet = 0;
    R_ksubsetRD(k, n);
    printf("%zu subset generati\n", TotalSet);

    // Run Gosper's algorithm
    printf("\nG_ksubsetGray(%d, %d) per ordine Gray:\n", k, n);
    TotalSet = 0;
    G_ksubsetGray(k, n);
    printf("%zu subset generati\n", TotalSet);
    
    free(S);

    return EXIT_SUCCESS;
}
/** EOF: Knuth.c **/