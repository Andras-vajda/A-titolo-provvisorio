/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/* Knuth.c - Optimal algorithms for k-subset generation               */
/*                                                                    */
/**********************************************************************/
/*          (c) 1990-2026 M.A.W. 1968 - CC BY-NC-SA / MIT             */
/* TECHGLISH: This file implements three different algorithms from    */
/* Donald Knuth's "The Art of Computer Programming" for generating    */
/* all k-subsets of an n-set: Algorithm T (colex on increasing tuples), */
/* Algorithm R (minimal-change "revdoor" order), and Gosper's         */
/* algorithm for bitwise subset generation.                           */
/**********************************************************************/
/* NERDYTALIAN: Questo file implementa tre diversi algoritmi dal      */
/* "The Art of Computer Programming" di Donald Knuth per generare     */
/* tutti i k-sottoinsiemi: Algoritmo T (colex sulle tuple crescenti), */
/* Algoritmo R (ordine "revdoor" a minima variazione)                 */
/* e l'algoritmo di Gosper per la generazione bitwise di sottoinsiemi.*/
/**********************************************************************/
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>

// Parse an unsigned decimal size without accepting overflow or trailing text.
static int parse_size(const char *text, size_t *value) {
    char *end;
    uintmax_t parsed;
    if (*text == '\0') return 0;
    for (const char *p = text; *p; ++p)
        if (*p < '0' || *p > '9') return 0;
    errno = 0;
    parsed = strtoumax(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || parsed > SIZE_MAX) return 0;
    *value = (size_t)parsed;
    return 1;
}


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
uint64_t TotalSet;

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
    printf("%5" PRIu64 " {%d, ", TotalSet++, S[0]);
    for (size_t i = 1; i < k -1; ++i) {
        printf("%d, ", S[i]);
    }
    printf("%d}\n", S[k -1]);
#else
    (void)k;
    TotalSet++;
#endif
}

/******************* ALGORITHM T: COLEX *******************/
/* TECHGLISH: Colex order on increasing tuples. For the explanation     */
/* and invariants, see the article and TAOCP 7.2.1.3, Algorithm T.      */
/*                                                                      */
/* NERDYTALIAN: Ordine colex sulle tuple crescenti. Per spiegazione     */
/* e invarianti, si vedano il testo e TAOCP 7.2.1.3, Algoritmo T.       */
/*************************************************************************/

void T_ksubsetLex(const size_t k, const size_t n) {
    int j, x;
    for (j = 0; j < (int)k; j++) {
        S[j] = j;
    }
    j = k;
    S[k] = n;
    S[k+1] = 0;

visit:
    printSubset(k);
    if (j > 0) {
        x = j;
        goto increment;
    }
    if (S[0] + 1 < S[1]) {
        S[0]++;
        goto visit;
    }
    else {
        j = 2;
    }

find_j:
    S[j - 2] = j - 2;
    x = S[j-1] + 1;
    if (x == S[j]) {
        j++;
        goto find_j;
    }
    if (j > (int)k) {
        return;
    }

increment:
    S[j-1] = x;
    j--;
    goto visit;
}

/******************* ALGORITHM R: REVOLVING DOOR *******************/
/* TECHGLISH: One removal and one addition per transition               */
/* (Hamming distance 2). For the explanation and invariants,            */
/* see the article and TAOCP 7.2.1.3, Algorithm R.                      */
/*                                                                      */
/* NERDYTALIAN: Una rimozione e una aggiunta per transizione            */
/* (distanza di Hamming 2). Per spiegazione e invarianti,               */
/* si vedano il testo e TAOCP 7.2.1.3, Algoritmo R.                     */
/*************************************************************************/

void R_ksubsetRD(const size_t k, const size_t n) {
    int j;
    for (j = 0; j < (int)k; j++) {
        S[j] = j;
    }
    S[k] = n;

visit:
    printSubset(k);
    if (k & 1) {
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
    if (S[j-1] >= j) {
        S[j-1] = S[j-2];
        S[j-2] = j - 2;
        goto visit;
    }
    else {
        j++;
    }

increase:
    if (S[j-1] + 1 < S[j]) {
        S[j-2] = S[j-1];
        S[j-1]++;
        goto visit;
    }
    else {
        j++;
        if (j <= (int)k) goto decrease;
    }
}

/******************* GOSPER'S ALGORITHM *************************/
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

// Contract: 3 <= k <= MAX_K, k < n <= 64; S has k+3 entries.
void G_ksubsetGosper(const size_t k, const size_t n) {
    // Initialize with first k-subset:
    // binary representation 00...0011...11 (k ones)
    uint64_t x = (1ULL << k) - 1;
    // Stop at the last mask; no shift by n, including when n == 64.
    uint64_t last = x << (n - k);

    for (;;) {
        /* Convert bit representation to integer array for output */
        size_t j = 0;
        uint64_t mask = UINT64_C(1);
        for (size_t i = 0; i < n; ++i) {
            if (x & mask) {
                S[j++] = i;
            }
            mask <<= 1;
        }
        printSubset(k);

        if (x == last) break;
        /* Generate the next k-subset using Gosper's hack */
        uint64_t u = x & -x;           // Isolate rightmost 1-bit
        uint64_t v = x + u;            // Turn off rightmost 1-bit
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

    if (!parse_size(argv[1], &k) || !parse_size(argv[2], &n)) {
        fputs("Errore: servono due interi decimali non negativi.\n", stderr);
        return EXIT_FAILURE;
    }
    if (n > 64) {
        fputs("Questa dimostrazione con Gosper richiede n <= 64.\n", stderr);
        return EXIT_FAILURE;
    }

    // Controllo valore minimo di k
    if (k < 3) {
        fprintf(stderr, "k deve essere maggiore di 2.\n");
        fprintf(stderr, "Uso: %s k n, con k < n\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Controllo valore massimo di k (mera programmazione difensiva)
    if (k > MAX_K) {
        fprintf(stderr, "k deve essere minore o uguale a %u "
                "per questa implementazione.\n", MAX_K);
        return EXIT_FAILURE;
    }

    if (k >= n) {
        fprintf(stderr, "Questa implementazione richiede k < n "
                "(ricevuti k=%zu, n=%zu).\n", k, n);
        fprintf(stderr, "Uso: %s k n, con k < n\n", argv[0]);
        return EXIT_FAILURE;
    }

    S = (int*)malloc((k + 3) * sizeof(int));
    if (NULL == S)
    {
        fputs("Errore di allocazione!\n", stderr);
        return EXIT_FAILURE;
    }

    // Run Algorithm T (colex order on increasing tuples)
    printf("T_ksubsetLex(%zu, %zu) per ordine colex "
           "(lex con indici decrescenti):\n", k, n);
    TotalSet = 0;
    T_ksubsetLex(k, n);
    printf("%" PRIu64 " subset generati\n", TotalSet);

    // Run Algorithm R (revdoor/Gray code order)
    printf("\nR_ksubsetRD(%zu, %zu) per ordine revdoor (Gray):\n", k, n);
    TotalSet = 0;
    R_ksubsetRD(k, n);
    printf("%" PRIu64 " subset generati\n", TotalSet);

    // Run Gosper's algorithm
    printf("\nG_ksubsetGosper(%zu, %zu):\n", k, n);
    TotalSet = 0;
    G_ksubsetGosper(k, n);
    printf("%" PRIu64 " subset generati\n", TotalSet);

    free(S);

    return EXIT_SUCCESS;
}
/** EOF: Knuth.c **/
