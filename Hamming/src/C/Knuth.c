/*
** Implementazione dei due algoritmi maggiormente efficienti
** ad oggi noti per la generazione di combinazioni e 
** k-sottoinsiemi, implementati come array.
** Dal volume 4 del TAOCP, 7.2.1.3, algoritmi T ed R.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ENABLE_OUTPUT

/* Puntatore al sottoinsieme generato */
int *S;

/* Totalizzatore k-sottoinsiemi generati - binomiale(n,k) */
size_t TotalSet;

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

/*
** Algoritmo 7.2.1.3.T: generazione in ordine lessicografico.
** Offre prestazioni ottimali, soprattutto al crescere di n.
*/
void T_ksubsetLex(const size_t k, const size_t n) {
    int j, x;

init:
    for (j = 0; j < k; j++) {
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

done:
    if (j > k) {
        return;
    }

increment:
    S[j-1] = x;
    j--;
    goto visit;
}


/*
** Algoritmo 7.2.1.3.R: generazione in ordine "revdoor"
** (Gray a minima variazione).
*/
void R_ksubsetRD(const size_t k, const size_t n) {
    int j;

init:
    for (j = 0; j < k; j++) {
        S[j] = j;
    }

    S[k] = n;

visit:
    printSubset(k);

easy:
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
        if (j <= k) goto decrease;
    }
}

/* Algoritmo di Gosper: generazione in ordine Gray */
void G_ksubsetGray(const size_t k, const size_t n) {
    uint64_t x = (1ULL << k) - 1; // Inizializza con i primi k bit a 1
    uint64_t limit = 1ULL << n;
    
    while (x < limit) {
        /* Estrai gli indici dalla rappresentazione bitwise */
        size_t j = 0, mask = 1;
        for (size_t i = 0; i < n; ++i) {
            if (x & mask) {
                S[j++] = i;
            }
            mask <<= 1;
        }
        printSubset(k);
        
        /* Genera la prossima combinazione in ordine Gray */
        uint64_t u = x & -x;
        uint64_t v = x + u;
        x = v + (((v ^ x) / u) >> 2);
    }
}

int main(int argc, char* argv[]) {
    int k, n;

    if (3 != argc) {
        fprintf(stderr, "Uso: %s k n, con 2 < k < n\n", argv[0]);
        return EXIT_FAILURE;
    }

    k = atoi(argv[1]);
    n = atoi(argv[2]);

    if (k < 3) {
        fprintf(stderr, "k deve essere maggiore di 2.\n");
        fprintf(stderr, "Uso: %s k n, con k < n\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (k >= n) {
        fprintf(stderr, "Non esistono %d-sottoinsiemi di un insieme di cardinalita' %d.\n", k, n);
        fprintf(stderr, "Uso: %s k n, con k < n\n", argv[0]);
        return EXIT_FAILURE;
    }
        
    S = (int*)malloc((k + 3) * sizeof(int));
    if (NULL == S)
    {
        fputs("Errore di allocazione!\n", stderr);
        return EXIT_FAILURE;
    }

    printf("T_ksubsetLex(%d, %d) per ordine lex:\n", k, n);
    TotalSet = 0;
    T_ksubsetLex(k, n);
    printf("%zu subset generati\n", TotalSet);

    printf("\nR_ksubsetRD(%d, %d) per ordine revdoor (Gray):\n", k, n);
    TotalSet = 0;
    R_ksubsetRD(k, n);
    printf("%zu subset generati\n", TotalSet);

    printf("\nG_ksubsetGray(%d, %d) per ordine Gray:\n", k, n);
    TotalSet = 0;
    G_ksubsetGray(k, n);
    printf("%zu subset generati\n", TotalSet);
    
    free(S);

    return EXIT_SUCCESS;
}
/** EOF: Knuth.c **/