/********************************************************************************/
/*
** Progetto: QLCI - Quadrati Latini Commutativi e Idempotenti
**
** Scopo: generazione elementare di quadrati latini commutativi e 
**        idempotenti (QLCI).
**        Bonus per i lettori (omaggio della Ditta).
**
** Nome del modulo: QLCI.c
**   - Implementa un generatore di QLCI basato su schemi combinatori rotazionali.
**   - Genera QLCI per diverse dimensioni specificate a compile-time.
**   - Utilizza a titolo llustrativo best practice di allocazione.
**
**********************************************************************************
** Nota teorica fondamentale: NON esistono QLCI di ordine PARI.
**********************************************************************************
**
** METRICHE:
**   - Lines of Code (LOC)....................: 175
**   - Source Lines of Code (SLOC)............: 118
**   - Comment Lines (CLOC)...................: 57
**   - Comment to Code Ratio (CCR)............: 0.48
**   - Cyclomatic Complexity (CC).............: 7
**   - Function Count.........................: 4
**   - Average Function Length................: 18 SLOC
**
** Licenza: MIT License
** Copyright (C) 2000-2025 M.A.W. 1968
*/
/********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Array costante di valori dispari per generare QLCI di varie dimensioni.
 * I QLCI esistono SOLO per dimensioni dispari.
 * Nel caso non fosse chiaro a sufficienza, tutti i valori qui 
 * indicati DEVONO essere dispari.
 */
static const size_t DIMENSIONI_QLCI[] = {5, 7, 13, 19, 21, 29};
static const size_t NUM_DIMENSIONI = sizeof(DIMENSIONI_QLCI) / sizeof(DIMENSIONI_QLCI[0]);

// Dichiarazione globale della matrice
static size_t **matrice = NULL;

/********************************************************************************/
/*
** alloca_matrice()
**
** Scopo:
**   - Allocare dinamicamente una matrice quadrata di dimensione n x n.
**   - Utilizza un'allocazione in blocco contiguo per efficienza e
**     per semplificare la gestione della memoria.
**
** Parametri:
**   - size_t n: La dimensione della matrice quadrata.
**
** Valore restituito:
**   - Nessuno. La funzione aggiorna la variabile globale 'matrice'.
**
** Errori:
**   - In caso di errore di allocazione, stampa un messaggio e termina il programma.
*/
/********************************************************************************/
void alloca_matrice(size_t n) {
    // Allocazione dell'array di puntatori alle righe
    matrice = (size_t **)malloc(n * sizeof(size_t *));
    if (NULL == matrice) {
        fprintf(stderr, "Errore: impossibile allocare memoria per l'array di puntatori.\n");
        exit(EXIT_FAILURE);
    }

    // Allocazione del blocco contiguo per tutti gli elementi
    size_t *dati = (size_t *)malloc(n * n * sizeof(size_t));
    if (NULL == dati) {
        fprintf(stderr, "Errore: impossibile allocare memoria per il blocco di dati.\n");
        free(matrice); // Libera il primo blocco allocato
        exit(EXIT_FAILURE);
    }

    // Collegamento delle righe al blocco contiguo
    for (size_t i = 0; i < n; i++) {
        matrice[i] = dati + i * n;
    }
}

/********************************************************************************/
/*
** libera_matrice()
**
** Scopo:
**   - Deallocare la memoria utilizzata dalla matrice.
**   - Funzione ideale per l'uso con atexit() per garantire la pulizia
**     delle risorse in caso di uscita anticipata.
**
** Parametri:
**   - Nessuno. La funzione opera sulla variabile globale 'matrice'.
**
** Valore restituito:
**   - Nessuno.
**
** Note:
**   - La funzione gestisce il caso in cui la matrice sia già NULL.
*/
/********************************************************************************/
void libera_matrice(void) {
    if (NULL != matrice) {
        free(matrice[0]); // Dealloca il blocco contiguo
        free(matrice);    // Dealloca l'array di puntatori
        matrice = NULL;   // Evita double free in esecuzioni successive
    }
}

/********************************************************************************/
/*
** stampa_matrice()
**
** Scopo:
**   - Visualizzare la matrice quadrata su stdout.
**   - Formatta l'output per una migliore leggibilità.
**
** Parametri:
**   - size_t n: La dimensione della matrice quadrata.
**
** Valore restituito:
**   - Nessuno.
*/
/********************************************************************************/
void stampa_matrice(size_t n) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            printf("%2zu ", matrice[i][j]);
        }
        printf("\n");
    }
}

/********************************************************************************/
/*
** genera_qlci()
**
** Scopo:
**   - Generare un Quadrato Latino Commutativo Idempotente (QLCI) di ordine n.
**   - Utilizza una tecnica di generazione basata su pattern rotazionali.
**
** Parametri:
**   - size_t n: L'ordine del QLCI da generare (deve essere dispari).
**
** Valore restituito:
**   - Nessuno. La funzione popola la matrice globale.
**
** Algoritmo:
**   - Popola la prima riga con un pattern specifico "a pettine".
**   - Genera le righe successive mediante rotazione ciclica a sinistra.
**   - Questo garantisce le proprietà di commutatività e idempotenza.
*/
/********************************************************************************/
void genera_qlci(size_t n) {
    // Calcolo dell'offset per le due serie "a pettine"
    // n è garantito dispari, quindi (n+1) è sempre pari e la divisione è esatta
    size_t meta_n = (n + 1) >> 1;  

    // Popola la prima riga
    for (size_t i = 0; i < n; i++) {
        // Utilizza shift e mascheramento bit per massima efficienza
        matrice[0][i] = (i >> 1) + 1 + ((i & 1) * meta_n);
    }

    // Popola le righe successive mediante rotazione di una 
    // posizione a sinistra per ogni riga
    for (size_t i = 1; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            matrice[i][j] = matrice[0][(j + i) % n];
        }
    }
}

/********************************************************************************/
/*
** main()
**
** Scopo:
**   - Punto di ingresso principale del programma.
**   - Genera e visualizza QLCI per varie dimensioni predefinite.
**
** Parametri:
**   - Nessuno (argc e argv non utilizzati).
**
** Valore restituito:
**   - EXIT_SUCCESS in caso di esecuzione normale.
**
** Note:
**   - Allocazione/deallocazione esplicita dopo ogni QLCI generato.
**   - Evita operazioni di divisione costose (DIV, IDIV, %) quando possibile.
*/
/********************************************************************************/
int main(void) {
    printf("Generatore di Quadrati Latini Commutativi Idempotenti (QLCI)\n");
    printf("===========================================================\n\n");
    
    // Itera attraverso tutte le dimensioni predefinite
    for (size_t idx = 0; idx < NUM_DIMENSIONI; idx++) {
        size_t n = DIMENSIONI_QLCI[idx];
        
        // Controllo ridondante per garantire che n sia dispari
        // Utilizzo dell'operazione bitwise atomica invece della costosa divisione
        if (0 == (n & 1)) {
            fprintf(stderr, "Errore: dimensione %zu non valida. "
                    "I QLCI esistono solo per dimensioni dispari.\n", n);
            continue;  // Salta questa iterazione
        }
        
        // Allocazione della matrice
        alloca_matrice(n);
        
        // Generazione del QLCI
        genera_qlci(n);
        
        // Visualizzazione del risultato
        printf("QLCI di ordine %zu:\n", n);
        stampa_matrice(n);
        printf("\n");
        
        // Deallocazione dopo ogni generazione
        libera_matrice();
    }
    
    return EXIT_SUCCESS;
}
/** EOF: QLCI.c **/