/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/* Hamming_DL.c - K-subset generation by Hamming distance             */
/*                                                                    */
/**********************************************************************/
/*          (c) 1990-2026 M.A.W. 1968 - CC BY-NC-SA / MIT             */
/* TECHGLISH: This implementation generates k-subsets of n-sets,      */
/* classified by their Hamming distance from a reference subset.      */
/* The algorithm uses bitwise operations for optimal performance,     */
/* representing each subset as an integer where the i-th bit          */
/* indicates whether element i belongs to the subset.                 */
/**********************************************************************/
/* NERDYTALIAN: Questa implementazione genera k-sottoinsiemi di       */
/* n-insiemi, classificati secondo la loro distanza di Hamming        */
/* da un sottoinsieme di riferimento. L'algoritmo utilizza operazioni */
/* bit a bit per prestazioni ottimali, rappresentando ogni            */
/* sottoinsieme come un intero in cui il bit i-esimo indica se        */
/* l'elemento i appartiene al sottoinsieme.                           */
/**********************************************************************/
/**********************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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


/******************* TYPES AND CONSTANTS *************************/
/* TECHGLISH: We represent sets as bit vectors using a 32-bit    */
/* unsigned integer type. This limits the maximum universe size  */
/* to 32 elements, which is sufficient for most applications.    */
/*                                                               */
/* NERDYTALIAN: Rappresentiamo gli insiemi come vettori di bit   */
/* utilizzando un tipo intero senza segno a 32 bit. Questo       */
/* limita la dimensione massima dell'universo a 32 elementi,     */
/* che e' sufficiente per la maggior parte delle applicazioni.   */
/*****************************************************************/

// Definizione del tipo per rappresentare l'insieme
typedef uint32_t Set_t;

// Pensiamo in grande, ma non troppo.
// 32 bit saranno sufficienti, per ora!
#define MAX_N 32

// Totalizzatore insiemi generati
size_t counter = 0;

/******************* BIT MANIPULATION UTILITIES ******************/
/* TECHGLISH: Efficient macros for bit manipulation operations.  */
/* We use compiler-specific intrinsics when available for better */
/* performance, with a fallback to standard bitwise operations.  */
/*                                                               */
/* NERDYTALIAN: Macro efficienti per operazioni di manipolazione */
/* dei bit. Utilizziamo funzioni intrinseche specifiche del      */
/* compilatore quando disponibili per prestazioni migliori, con  */
/* un ripiego su operazioni bit a bit standard.                  */
/*****************************************************************/

// Una comoda macro per la stampa dei subset
#define HAS_MORE_BITS(value, i) ((value) & ~((UINT32_C(1) << ((i) + 1)) - 1))

/******************* OUTPUT FORMATTING FUNCTIONS *****************/
/* TECHGLISH: Functions to display subsets in human-readable     */
/* formats. print_set shows a subset in algebraic notation with  */
/* elements as letters, while print_binary shows the bit vector  */
/* representation along with metadata about the subset.          */
/*                                                               */
/* NERDYTALIAN: Funzioni per visualizzare i sottoinsiemi in      */
/* formati leggibili. print_set mostra un sottoinsieme in        */
/* notazione algebrica con elementi come lettere, mentre         */
/* print_binary mostra la rappresentazione del vettore di bit    */
/* insieme ai metadati sul sottoinsieme.                         */
/*****************************************************************/

// Funzione per la stampa di un sottoinsieme
void print_set(Set_t value, size_t n) {
    printf(" {");
    for (size_t i = 0; i < n; ++i) {
        if (value & (UINT32_C(1) << i)) {
            // Converte la posizione in carattere (a, b, c, ...)
            if (n <= 26) printf("%c", (int)('a' + i));
            else printf("u%zu", i); 
            if (i < n - 1 && HAS_MORE_BITS(value, i)) {
                // Aggiunge la virgola solo se ci sono altri elementi
                printf(", "); 
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
        printf("%4zu [%zu]: (%06" PRIX32 ") ", counter, d, value);
    }
    
    // Stampa il vettore booleano caratteristico
    for (size_t i = n; i > 0; --i) {
        printf("%c", (value & (UINT32_C(1) << (i - 1))) ? '1' : '0');
    }
    
    // Arricchisce l'output con una stampa del subset in formato simbolico
    print_set(v, n);
    puts("");
}

/******************* SUBSET GENERATION ALGORITHM ******************/
/* TECHGLISH: Gosper's algorithm for generating the next k-subset */
/* in increasing numeric (colex) order. This elegant bit manipulation algorithm*/
/* directly computes the successor without iteration, providing   */
/* optimal performance for combinatorial generation.              */
/*                                                                */
/* NERDYTALIAN: Algoritmo di Gosper per generare il prossimo      */
/* k-sottoinsieme in ordine numerico crescente (colex). Questo elegante       */
/* algoritmo di manipolazione dei bit calcola direttamente il     */
/* successore senza iterazione, fornendo prestazioni ottimali     */
/* per la generazione combinatoria.                               */
/******************************************************************/

// Genera il successore del subset corrente
Set_t next_subset(Set_t subset, Set_t limit) {
    if (subset == 0 || subset >= limit)
        return limit;
    // Isola il bit piu' a destra (LSB)
    Set_t smallest = subset & -subset;    
    // Propaga il riporto    
    Set_t ripple = subset + smallest;     
    // Identifica i bit cambiati    
    Set_t ones = subset ^ ripple;         
    // Riposiziona i bit a 1    
    ones = (ones / smallest) >> 2;        
    // Ricombina il risultato con un OR   
    return ripple | ones;                 
}

/******************* HAMMING DISTANCE GENERATION ******************/
/* TECHGLISH: This core function generates all k-subsets at a     */
/* specific Johnson distance d from a reference subset. The       */
/* algorithm works by systematically removing d elements from     */
/* the base subset and adding d new elements, exploring all       */
/* possible combinations of additions and removals.               */
/*                                                                */
/* NERDYTALIAN: Questa funzione fondamentale genera tutti i       */
/* k-sottoinsiemi a una semidistanza di Hamming d specifica da un     */
/* sottoinsieme di riferimento. L'algoritmo funziona rimuovendo   */
/* sistematicamente d elementi dal sottoinsieme base e aggiungendo*/
/* d nuovi elementi, esplorando tutte le possibili combinazioni   */
/* di aggiunte e rimozioni.                                       */
/******************************************************************/

// Funzione per generare k-subset a distanza desiderata
void generate_k_subset(size_t n, size_t k, size_t d) {
    // Preconditions of this 32-bit implementation, before any shift.
    if (k < 2 || k > MAX_N / 2 || n < 2 * k || n > MAX_N || d > k) {
        fputs("Parametri non validi per generate_k_subset.\n", stderr);
        exit(EXIT_FAILURE);
    }
    // Crea base: subset = {0,1,...,k-1}    
    Set_t base_set = (UINT32_C(1) << k) - 1;        
    // Maschera iniziale per d bit    
    Set_t base_mask = (UINT32_C(1) << d) - 1;       

    if (d == 0) {
        counter = 1; // Count the reference subset as well.
        printf("** Base set: ");
        print_binary(base_set, n, d, false);
        return;
    }


    while (base_mask < (UINT32_C(1) << k)) {
        Set_t diff_mask = (UINT32_C(1) << d) - 1;
        while (diff_mask < (UINT32_C(1) << (n - k))) {
            // Rimuovi d elementi
            Set_t modified_base = base_set & ~base_mask;  
            // Aggiungi d nuovi elementi
            Set_t shifted_diff = diff_mask << k;   
            // Ricombina i valori con OR       
            Set_t result = modified_base | shifted_diff;  
            ++counter;            
            print_binary(result, n, d, true);
            // Prossima maschera
            diff_mask = next_subset(diff_mask, (UINT32_C(1) << (n - k))); 
            if (counter == SIZE_MAX) {
                fprintf(stderr, "Errore: raggiunto il limite massimo di conteggio. Terminazione.\n");
                exit(EXIT_FAILURE);
            }
        }
        // Prossima combinazione di bit da rimuovere
        base_mask = next_subset(base_mask, (UINT32_C(1) << k)); 
    }
}

/******************* COMPLETE SUBSET GENERATION ******************/
/* TECHGLISH: Generates all k-subsets at all possible Hamming    */
/* distances from the reference subset, organizing them by       */
/* increasing distance. This allows for a comprehensive view of  */
/* the combinatorial space partitioned by Hamming distance.      */
/*                                                               */
/* NERDYTALIAN: Genera i k-sottoinsiemi a tutte le possibili     */
/* semidistanze di Hamming dal sottoinsieme di riferimento,          */
/* organizzandoli per distanza crescente. Ciò consente una       */
/* visione completa dello spazio combinatorio partizionato       */
/* dalla distanza di Hamming.                                    */
/*****************************************************************/

// Generazione esaustiva di tutti i k-subset con 0 <= d <= k
void generate_all_subsets(size_t n, size_t k) {
    for (size_t d = 0; d <= k; ++d) {
        printf("\n=== d = %zu ===\n", d);
        generate_k_subset(n, k, d);
    }
}

/******************* MAIN PROGRAM **********************************/
/* TECHGLISH: Command-line interface for the subset generator.     */
/* Takes parameters n (universe size) and k (subset size), with    */
/* validation to ensure the constraints are met. Demonstrates      */
/* both targeted generation with a random distance and complete    */
/* generation of all subsets by distance.                          */
/*                                                                 */
/* NERDYTALIAN: Interfaccia a riga di comando per il generatore    */
/* di sottoinsiemi. Prende i parametri n (dimensione dell'universo)*/
/* e k (dimensione del sottoinsieme), con validazione per garantire*/
/* che i vincoli siano soddisfatti. Dimostra sia la generazione    */
/* mirata con una distanza casuale sia la generazione completa     */
/* di tutti i sottoinsiemi per distanza.                           */
/*******************************************************************/

int main(int argc, char *argv[]) {
    size_t n, k, d;
    
    // Verifica degli argomenti da riga di comando
    if (argc != 3) {
        fprintf(stderr, "** Uso: %s n k\n"
                "** k >= 2 e 2k <= n <= %d.\n", 
                argv[0], MAX_N);
        return EXIT_FAILURE;
    } 
        
    if (!parse_size(argv[1], &n) || !parse_size(argv[2], &k)) {
        fputs("Errore: servono due interi decimali non negativi.\n", stderr);
        return EXIT_FAILURE;
    }
    
    srand((unsigned int)time(NULL));
    
    // Validazione dei parametri
    if (k < 2 || k > MAX_N / 2 || n < 2 * k || n > MAX_N) {
        fprintf(stderr, "Errore: Assicurati che 2 <= k e 2k <= n <= %d.\n", MAX_N);
        return EXIT_FAILURE;
    }
    
    // FASE 1: Generazione con distanza casuale
    d = (rand() % k) + 1;
    printf("** FASE 1: Generazione dei %zu-subset a distanza casuale (%zu)\n", k, d);
    generate_k_subset(n, k, 0);   // Prima il subset base
    generate_k_subset(n, k, d);   // Poi i subset a distanza d

    // FASE 2: Generazione completa per ogni distanza
    printf("\n** FASE 2: Generazione esaustiva dei %zu-subset\n"
           "**         ordinati per semidistanza di Hamming crescente:\n", k);
    generate_all_subsets(n, k);
    printf("\n** Subset generati: %zu\n\n", counter);

    return EXIT_SUCCESS;
}
/** EOF: Hamming_DL.c **/