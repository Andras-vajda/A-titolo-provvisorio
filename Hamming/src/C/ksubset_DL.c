/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/* ksubset_DL.c - Unit Hamming distance subset generator              */
/*                                                                    */
/**********************************************************************/
/*          (c) 1990-2026 M.A.W. 1968 - CC BY-NC-SA / MIT             */
/* TECHGLISH: This implementation generates all k-subsets that are at */
/* Hamming semi-distance 1 from a reference subset. These are subsets */
/* that differ from the base subset by exactly one element - one      */
/* element is removed and one is added. The algorithm leverages       */
/* bitwise operations for efficiency.                                 */
/**********************************************************************/
/* NERDYTALIAN: Questa implementazione genera tutti i k-sottoinsiemi  */
/* che si trovano a semi-distanza di Hamming 1 da un sottoinsieme di  */
/* riferimento. Questi sono sottoinsiemi che differiscono dal         */
/* sottoinsieme base per esattamente un elemento - un elemento viene  */
/* rimosso e uno viene aggiunto. L'algoritmo sfrutta operazioni       */
/* bit a bit per efficienza.                                          */
/**********************************************************************/
/**********************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
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


/******************* CONSTANTS ***********************************/
/* TECHGLISH: The maximum universe size is limited by the number */
/* of bits in a 64-bit integer (uint64_t), setting an upper      */
/* bound of 64 elements. This is sufficient for most practical   */
/* applications without requiring multi-word representations.    */
/*                                                               */
/* NERDYTALIAN: La dimensione massima dell'universo e' limitata  */
/* dal numero di bit in un intero a 64 bit (uint64_t), impostando*/
/* un limite superiore di 64 elementi. Questo e' sufficiente per */
/* la maggior parte delle applicazioni pratiche senza richiedere */
/* rappresentazioni multi-parola.                                */
/*****************************************************************/

#define MAX_N 64 // Massima cardinalita' dell'universo
// Limite pratico per k (evita tempi di elaborazione eccessivi)
#define MAX_K 32

/******************* OUTPUT FUNCTION *****************************/
/* TECHGLISH: Displays a subset in human-readable format, where  */
/* the subset is represented as a bit vector. For each position  */
/* with a bit set to 1, the corresponding element (using 1-based */
/* indexing for output) is displayed within curly braces.        */
/*                                                               */
/* NERDYTALIAN: Visualizza un sottoinsieme in formato leggibile, */
/* dove il sottoinsieme e' rappresentato come un vettore di bit. */
/* Per ogni posizione con un bit impostato a 1, l'elemento       */
/* corrispondente (utilizzando l'indicizzazione 1-based per      */
/* l'output) viene visualizzato tra parentesi graffe.            */
/*****************************************************************/

void printSubset(uint64_t B, size_t n) {
    printf("{ ");
    for (size_t i = 0; i < n; ++i) {
        if (B & (1ULL << i)) {
            printf("%zu ", i + 1);  // Off-by-one in output
        }
    }
    printf("}\n");
}

/******************* UNIT DISTANCE GENERATOR **********************/
/* TECHGLISH: Generates all k-subsets at Hamming semi-distance 1  */
/* from a reference subset. The algorithm works by systematically */
/* replacing each element in the base subset with each element    */
/* not in the base subset, producing exactly k*(n-k) new subsets. */
/*                                                                */
/* NERDYTALIAN: Genera tutti i k-sottoinsiemi a semi-distanza di  */
/* Hamming 1 da un sottoinsieme di riferimento. L'algoritmo       */
/* funziona sostituendo sistematicamente ogni elemento nel        */
/* sottoinsieme base con ogni elemento non nel sottoinsieme base, */
/* producendo esattamente k*(n-k) nuovi sottoinsiemi.             */
/******************************************************************/

void generateHammingDistance1(size_t k, size_t n) {
    // Contatore per i sottoinsiemi generati
    size_t counter = 0;

    // Inizializza il subset base con i primi k bit impostati a 1
    // Es. per k = 3: B = 0b111 = 7 = 2^3 -1
    uint64_t B = (1ULL << k) - 1;

    // Stampa il subset base per riferimento
    printf("Base subset:\n");
    printSubset(B, n);

    // Genera tutti i subset a semidistanza di Hamming unitaria
    for (size_t i = 0; i < k; ++i) {
        // Azzera il bit i-esimo del subset base (rimuovi elemento i)
        uint64_t clear_mask = ~(1ULL << i);

        // Itera su tutti gli elementi non nel subset base
        for (size_t j = k; j < n; ++j) {
            // Imposta il bit j-esimo (aggiungi elemento j)
            uint64_t set_mask = (1ULL << j);

            // Modifica il subset: rimuovi i, aggiungi j
            uint64_t modified = (B & clear_mask) | set_mask;

            // Incrementa il contatore e stampa il subset
            ++counter;
            printSubset(modified, n);

            // Controllo overflow del contatore
            if (counter == SIZE_MAX) {
                fprintf(stderr, "Errore: raggiunto il limite massimo "
                        "di conteggio. Terminazione.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    printf("\nTotale subset generati: %zu\n", counter);
}

/******************* MAIN PROGRAM *******************************/
/* TECHGLISH: Command-line interface that validates parameters   */
/* and executes the generation algorithm. Takes k (subset size)  */
/* and n (universe size) as parameters, ensuring that they meet  */
/* the necessary constraints before proceeding with generation.  */
/*                                                               */
/* NERDYTALIAN: Interfaccia a riga di comando che convalida i    */
/* parametri ed esegue l'algoritmo di generazione. Prende k      */
/* (dimensione del sottoinsieme) e n (dimensione dell'universo)  */
/* come parametri, assicurandosi che soddisfino i vincoli        */
/* necessari prima di procedere con la generazione.              */
/*****************************************************************/

int main(int argc, char *argv[]) {
    // Argomenti da linea di comando
    if (argc != 3) {
        fprintf(stderr, "Uso: %s k n\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t k, n;
    if (!parse_size(argv[1], &k) || !parse_size(argv[2], &n)) {
        fputs("Errore: servono due interi decimali non negativi.\n", stderr);
        return EXIT_FAILURE;
    }

    // Convalida dei parametri
    if (k < 3) {
        fprintf(stderr, "Errore: k deve essere maggiore di 2.\n");
        return EXIT_FAILURE;
    }

    if (k > MAX_K) {
        fprintf(stderr, "Errore: k deve essere minore o uguale a %d "
                "per questa implementazione.\n", MAX_K);
        return EXIT_FAILURE;
    }

    if (n < 2 * k) {
        fprintf(stderr, "Errore: n deve essere almeno 2k (n >= %zu).\n",
                2 * k);
        return EXIT_FAILURE;
    }

    if (n > MAX_N) {
        fprintf(stderr, "Errore: n deve essere minore o uguale a %d "
                "per questa implementazione.\n", MAX_N);
        return EXIT_FAILURE;
    }

    // Genera e stampa tutti i sottoinsiemi a semidistanza Hamming unitaria
    generateHammingDistance1(k, n);

    return EXIT_SUCCESS;
}
/** EOF: ksubset_DL.c **/
