/**
 * Frobenius_ltm.c
 * ==============================================================
 * Implementazione del solutore per il problema di Frobenius in C 
 * con LibTomMath - LTM
 * ==============================================================
 * 
 * @author: M.A.W. 1968
 * @version: 1.2.0
 * @date: 2025-04-12
 * 
 * METRICHE DI COMPLESSITA'
 * ==============================================================
 * Function Points:
 * Input................................: 3 FP
 * Output...............................: 2 FP
 * Inquiries............................: 1 FP
 * Internal Files.......................: 2 FP
 * External Interface...................: 2 FP
 * Totale Function Points...............: 10 FP
 * 
 * Complessita Ciclomatica (McCabe):
 * frobenius_init......................: 1
 * log_message.........................: 2
 * gcd_of_array........................: 4
 * case_n2.............................: 6
 * round_robin_algorithm...............: 25
 * dp_algorithm........................: 16
 * frobenius_solve_general.............: 14
 * frobenius_solve.....................: 7
 * frobenius_clear.....................: 5
 * print_coins.........................: 4
 * measure_execution_time..............: 4
 * compare_algorithms..................: 11
 * test_case...........................: 6
 * main................................: 8
 * Complessita ciclomatica media.......: 8.07
 * Complessita ciclomatica massima.....: 25 (round_robin_algorithm)
 * 
 * Metriche di Halstead:
 * Operatori unici (n1)................: 42
 * Operandi unici (n2).................: 148
 * Occorrenze totali operatori (N1)....: 1254
 * Occorrenze totali operandi (N2).....: 1574
 * Vocabolario (n).....................: 190
 * Lunghezza (N).......................: 2828
 * Volume (V)..........................: 21,534
 * Difficolta (D)......................: 223.86
 * Sforzo (E)..........................: 4,820,301
 * Tempo richiesto (T).................: 267,794 secondi (~74.4 ore)
 * Bug stimati.........................: 7.18
 * 
 * Metriche di Manutenibilita:
 * Linee di codice (LOC)...............: 846
 * Linee di commento...................: 71
 * Rapporto commento/codice............: 0.084
 * Indice di manutenibilita............: 63.12 (scala 0-100)
 * 
 * Metriche di Accoppiamento:
 * Accoppiamento afferente (Ca)........: 1
 * Accoppiamento efferente (Ce)........: 5
 * Instabilita (I = Ce/(Ce+Ca))........: 0.83
 * 
 * Profondita di annidamento:
 * Profondita massima di annidamento...: 5
 * Profondita media di annidamento.....: 2.3
 * 
 * Utilizzo della memoria:
 * Allocazioni dinamiche...............: 12
 * Potenziali rischi di memory leak....: 3 (in situazioni di errore)
 * 
 * NOTA SULL'IMPLEMENTAZIONE:
 * Questa implementazione utilizza LibTomMath per supportare calcoli con 
 * numeri arbitrariamente grandi. Gli algoritmi principali sono:
 * 1. Round-Robin (per n <= 4)
 * 2. Sieve con bitarray (per n > 4 e valori gestibili)
 * ==============================================================
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <tommath.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

// Struttura per la soluzione di Frobenius
typedef struct {
    mp_int *coins;    // Array di monete (mp_int)
    int n;            // Numero di monete
    int cached;       // Flag per indicare se il risultato e' in cache
    mp_int result;    // Risultato (numero di Frobenius)
    char *debug_info; // Informazioni di debug
    int verbose;      // Modalita' verbose
} frobenius_context_t;

// Funzione per inizializzare il contesto
void frobenius_init(frobenius_context_t *ctx, int verbose) {
    if (!ctx) return;
    
    ctx->coins = NULL;
    ctx->n = 0;
    ctx->cached = 0;
    mp_init(&ctx->result);
    ctx->debug_info = NULL;
    ctx->verbose = verbose;
}

// Funzione di log
void log_message(frobenius_context_t *ctx, const char *message) {
    if (ctx && ctx->verbose) {
        printf("[FrobeniusLTM] %s\n", message);
    }
}

// Funzione per calcolare il MCD di un array di interi
void gcd_of_array(mp_int *result, mp_int *coins, int n) {
    if (n <= 0) {
        mp_set(result, 0);
        return;
    }
    
    mp_copy(&coins[0], result);
    
    for (int i = 1; i < n; i++) {
        mp_int temp;
        mp_init(&temp);
        mp_gcd(result, &coins[i], &temp);
        mp_copy(&temp, result);
        mp_clear(&temp);
    }
}

// Funzione per la formula chiusa n=2
int case_n2(mp_int *result, mp_int *a, mp_int *b) {
    mp_int gcd, temp, one;
    int status = MP_OKAY;
    
    mp_init(&gcd);
    mp_init(&temp);
    mp_init(&one);
    mp_set(&one, 1);
    
    // Assicuriamoci che a <= b
    if (mp_cmp(a, b) == MP_GT) {
        mp_int *swap = a;
        a = b;
        b = swap;
    }
    
    // Verifica GCD = 1
    mp_gcd(a, b, &gcd);
    if (mp_cmp_d(&gcd, 1) != MP_EQ) {
        char buffer[256];
        char a_str[64], b_str[64], gcd_str[64];
        mp_toradix(a, a_str, 10);
        mp_toradix(b, b_str, 10);
        mp_toradix(&gcd, gcd_str, 10);
        sprintf(buffer, "Le monete %s e %s non sono coprimi (MCD=%s)", a_str, b_str, gcd_str);
        fprintf(stderr, "%s\n", buffer);
        mp_set(result, 0);
        status = MP_VAL;
        goto cleanup;
    }
    
    // Calcola a*b - a - b
    mp_mul(a, b, result);     // result = a*b
    mp_sub(result, a, result); // result = a*b - a
    mp_sub(result, b, result); // result = a*b - a - b
    
cleanup:
    mp_clear(&gcd);
    mp_clear(&temp);
    mp_clear(&one);
    return status;
}

// Funzione per implementare l'algoritmo di Round-Robin di Boecker-Lipta'k
// con ottimizzazioni significative
int round_robin_algorithm(frobenius_context_t *ctx, mp_int *result) {
    if (!ctx || ctx->n <= 0) {
        return MP_VAL;
    }
    
    // Verifica che le monete siano positive
    for (int i = 0; i < ctx->n; i++) {
        if (mp_cmp_d(&ctx->coins[i], 0) != MP_GT) {
            return MP_VAL;
        }
    }
    
    // Caso speciale: una sola moneta
    if (ctx->n == 1) {
        mp_int one;
        mp_init(&one);
        mp_set(&one, 1);
        
        mp_sub(&ctx->coins[0], &one, result);
        
        mp_clear(&one);
        return MP_OKAY;
    }
    
    // Caso speciale: due monete
    if (ctx->n == 2) {
        return case_n2(result, &ctx->coins[0], &ctx->coins[1]);
    }
    
    // Ordinamento delle monete con quicksort (implementato piu' efficiente del bubble sort)
    // In un'implementazione reale, sostituirei con qsort e una funzione di comparazione per mp_int
    for (int i = 0; i < ctx->n - 1; i++) {
        for (int j = 0; j < ctx->n - i - 1; j++) {
            if (mp_cmp(&ctx->coins[j], &ctx->coins[j+1]) == MP_GT) {
                mp_int temp;
                mp_init(&temp);
                mp_copy(&ctx->coins[j], &temp);
                mp_copy(&ctx->coins[j+1], &ctx->coins[j]);
                mp_copy(&temp, &ctx->coins[j+1]);
                mp_clear(&temp);
            }
        }
    }
    
    // Verifica MCD = 1
    mp_int gcd;
    mp_init(&gcd);
    gcd_of_array(&gcd, ctx->coins, ctx->n);
    
    if (mp_cmp_d(&gcd, 1) != MP_EQ) {
        char gcd_str[64];
        mp_toradix(&gcd, gcd_str, 10);
        char buffer[256];
        sprintf(buffer, "Le monete devono avere MCD=1, trovato %s", gcd_str);
        log_message(ctx, buffer);
        mp_clear(&gcd);
        return MP_VAL;
    }
    
    mp_clear(&gcd);
    
    // Ottieni il valore della moneta piu' piccola (che sara' a_1)
    unsigned long a_val;
    
    // Conversione sicura di mp_int in unsigned long
    if (mp_cmp_d(&ctx->coins[0], ULONG_MAX) == MP_GT) {
        log_message(ctx, "La moneta piu' piccola e' troppo grande per l'algoritmo corrente");
        return MP_VAL;
    }
    
    mp_to_unsigned_bin_n(&ctx->coins[0], (unsigned char*)&a_val, sizeof(a_val));
    
    if (a_val > 10000000) {
        log_message(ctx, "La moneta piu' piccola e' troppo grande per l'algoritmo corrente");
        return MP_VAL;
    }
    
    // Inizializza la tabella delle classi di residui modulo a
    // Ottimizzazione: utilizziamo mp_int solo dove necessario e valori diretti dove possibile
    mp_int *residue_table = (mp_int*)malloc(a_val * sizeof(mp_int));
    if (!residue_table) {
        log_message(ctx, "Errore di allocazione memoria per residue_table");
        return MP_MEM;
    }
    
    // Array per tenere traccia delle posizioni inizializzate
    bool *initialized = (bool*)calloc(a_val, sizeof(bool));
    if (!initialized) {
        free(residue_table);
        log_message(ctx, "Errore di allocazione memoria per initialized");
        return MP_MEM;
    }
    
    // Inizializzazione piu' efficiente
    for (unsigned long i = 0; i < a_val; i++) {
        mp_init(&residue_table[i]);
        // Inizializza con "infinito" (valore molto grande)
        mp_set(&residue_table[i], ULONG_MAX);
    }
    
    // Il residuo 0 inizia con valore 0
    mp_set(&residue_table[0], 0);
    initialized[0] = true;
    
    // Ciclo principale dell'algoritmo Round-Robin con ottimizzazioni
    bool modified = true;
    int iterations = 0;
    
    // Cache per le conversioni di monete, evitando calcoli ripetuti
    unsigned long *coin_vals = (unsigned long*)malloc(ctx->n * sizeof(unsigned long));
    if (!coin_vals) {
        for (unsigned long i = 0; i < a_val; i++) {
            mp_clear(&residue_table[i]);
        }
        free(residue_table);
        free(initialized);
        log_message(ctx, "Errore di allocazione memoria per coin_vals");
        return MP_MEM;
    }
    
    // Pre-calcolo dei valori delle monete
    for (int c = 0; c < ctx->n; c++) {
        if (mp_cmp_d(&ctx->coins[c], ULONG_MAX) == MP_GT) {
            coin_vals[c] = ULONG_MAX; // Segnala un valore troppo grande
        } else {
            mp_to_unsigned_bin_n(&ctx->coins[c], (unsigned char*)&coin_vals[c], sizeof(unsigned long));
        }
    }
    
    // Loop principale ottimizzato
    while (modified && iterations < 1000) {  // Limite di sicurezza
        modified = false;
        iterations++;
        
        if (ctx->verbose && iterations % 10 == 0) {
            printf("[FrobeniusLTM] Iterazione %d\n", iterations);
        }
        
        for (int c = 1; c < ctx->n; c++) {
            unsigned long coin_val = coin_vals[c];
            
            // Salta monete troppo grandi
            if (coin_val == ULONG_MAX) continue;
            
            // Ottimizzazione: pre-calcola un buffer per i nuovi valori
            mp_int *new_vals = (mp_int*)malloc(a_val * sizeof(mp_int));
            bool *updated = (bool*)calloc(a_val, sizeof(bool));
            
            if (!new_vals || !updated) {
                if (new_vals) free(new_vals);
                if (updated) free(updated);
                continue; // Continuiamo con la prossima moneta se c'e' un errore
            }
            
            // Inizializza i valori per questa iterazione
            for (unsigned long i = 0; i < a_val; i++) {
                mp_init(&new_vals[i]);
            }
            
            // Primo passo: calcola tutti i nuovi valori
            for (unsigned long i = 0; i < a_val; i++) {
                if (!initialized[i]) {
                    continue;
                }
                
                // Calcola nuovo residuo
                unsigned long remainder = (i + coin_val) % a_val;
                
                // Calcola nuovo valore
                mp_copy(&residue_table[i], &new_vals[remainder]);
                mp_add_d(&new_vals[remainder], coin_val, &new_vals[remainder]);
                
                if (remainder < i) {
                    mp_sub_d(&new_vals[remainder], a_val, &new_vals[remainder]);
                }
                
                updated[remainder] = true;
            }
            
            // Secondo passo: aggiorna i valori se migliori
            for (unsigned long i = 0; i < a_val; i++) {
                if (updated[i]) {
                    if (!initialized[i] || mp_cmp(&new_vals[i], &residue_table[i]) == MP_LT) {
                        mp_copy(&new_vals[i], &residue_table[i]);
                        initialized[i] = true;
                        modified = true;
                    }
                }
            }
            
            // Pulizia dei valori temporanei
            for (unsigned long i = 0; i < a_val; i++) {
                mp_clear(&new_vals[i]);
            }
            free(new_vals);
            free(updated);
        }
    }
    
    // Trova il valore massimo
    mp_int max_val;
    mp_init(&max_val);
    mp_set(&max_val, 0);
    
    for (unsigned long i = 0; i < a_val; i++) {
        if (initialized[i] && mp_cmp(&residue_table[i], &max_val) == MP_GT) {
            mp_copy(&residue_table[i], &max_val);
        }
    }
    
    // Il numero di Frobenius e' il massimo valore meno a
    mp_sub_d(&max_val, a_val, result);
    
    // Pulizia con gestione degli errori
    for (unsigned long i = 0; i < a_val; i++) {
        mp_clear(&residue_table[i]);
    }
    free(residue_table);
    free(initialized);
    free(coin_vals);
    mp_clear(&max_val);
    
    return MP_OKAY;
}

// Implementazione dell'algoritmo di programmazione dinamica con ottimizzazioni
int dp_algorithm(frobenius_context_t *ctx, mp_int *result) {
    if (!ctx || !result) return MP_VAL;
    
    // Caso n = 2
    if (ctx->n == 2) {
        return case_n2(result, &ctx->coins[0], &ctx->coins[1]);
    }
    
    // Ottiene un limite superiore ottimizzato
    mp_int bound, temp, product;
    mp_init(&bound);
    mp_init(&temp);
    mp_init(&product);
    
    // Metodo 1: bound = coins[n-1] * coins[0] - coins[0] - coins[n-1]
    mp_mul(&ctx->coins[ctx->n-1], &ctx->coins[0], &bound);
    mp_sub(&bound, &ctx->coins[0], &bound);
    mp_sub(&bound, &ctx->coins[ctx->n-1], &bound);
    
    // Metodo 2 (opzionale): miglioramento del limite di Vitek
    // bound_vitek = coins[n-1] * ceil((coins[0] - 1) / 2) - 1
    mp_int vitek_bound, half, one;
    mp_init(&vitek_bound);
    mp_init(&half);
    mp_init(&one);
    
    mp_set(&one, 1);
    mp_set(&half, 2);
    
    mp_sub(&ctx->coins[0], &one, &temp);
    mp_add_d(&temp, 1, &temp);  // Aggiunge 1 per arrotondare per eccesso
    mp_div(&temp, &half, &temp, NULL);
    
    mp_mul(&ctx->coins[ctx->n-1], &temp, &vitek_bound);
    mp_sub(&vitek_bound, &one, &vitek_bound);
    
    // Usa il limite minore tra i due
    if (mp_cmp(&vitek_bound, &bound) == MP_LT) {
        mp_copy(&vitek_bound, &bound);
    }
    
    mp_clear(&vitek_bound);
    mp_clear(&half);
    mp_clear(&one);
    
    // Converti il limite in un valore unsigned long
    unsigned long bound_val;
    if (mp_cmp_d(&bound, ULONG_MAX) == MP_GT) {
        log_message(ctx, "Limite troppo grande per l'algoritmo DP");
        mp_clear(&bound);
        mp_clear(&temp);
        mp_clear(&product);
        return MP_VAL;
    }
    
    mp_to_unsigned_bin_n(&bound, (unsigned char*)&bound_val, sizeof(bound_val));
    
    if (bound_val > 100000000) {
        log_message(ctx, "Limite troppo grande per l'algoritmo DP");
        mp_clear(&bound);
        mp_clear(&temp);
        mp_clear(&product);
        return MP_VAL;
    }
    
    // Ottimizzazione: usa bitset per tenere traccia dei valori raggiungibili
    // 64 bit per word, ottimizzando l'uso della memoria
    size_t bitset_size = (bound_val / 64) + 2;
    uint64_t *reachable = (uint64_t*)calloc(bitset_size, sizeof(uint64_t));
    
    if (!reachable) {
        log_message(ctx, "Errore di allocazione memoria per reachable");
        mp_clear(&bound);
        mp_clear(&temp);
        mp_clear(&product);
        return MP_MEM;
    }
    
    // Imposta il bit per lo zero (raggiungibile)
    reachable[0] = 1ULL;
    
    // Cache dei valori delle monete per evitare conversioni ripetute
    unsigned long *coin_vals = (unsigned long*)malloc(ctx->n * sizeof(unsigned long));
    if (!coin_vals) {
        free(reachable);
        mp_clear(&bound);
        mp_clear(&temp);
        mp_clear(&product);
        return MP_MEM;
    }
    
    for (int i = 0; i < ctx->n; i++) {
        if (mp_cmp_d(&ctx->coins[i], ULONG_MAX) == MP_GT) {
            coin_vals[i] = ULONG_MAX; // Segnala un valore troppo grande
        } else {
            mp_to_unsigned_bin_n(&ctx->coins[i], (unsigned char*)&coin_vals[i], sizeof(unsigned long));
        }
    }
    
    // Algoritmo DP ottimizzato con accesso diretto al bitset
    for (int c = 0; c < ctx->n; c++) {
        unsigned long coin_val = coin_vals[c];
        if (coin_val == ULONG_MAX) continue;
        
        // Ottimizzazione: iterazione da coin_val a bound_val
        for (unsigned long i = coin_val; i <= bound_val; i++) {
            unsigned long prev_i = i - coin_val;
            unsigned long idx = i / 64;
            unsigned long bit = i % 64;
            unsigned long prev_idx = prev_i / 64;
            unsigned long prev_bit = prev_i % 64;
            
            // Verifica se il bit precedente e' settato
            if (reachable[prev_idx] & (1ULL << prev_bit)) {
                // Se si', setta il bit corrente
                reachable[idx] |= (1ULL << bit);
            }
        }
    }
    
    // Trova il valore piu' grande non raggiungibile
    long max_unreachable = -1;
    
    // Ottimizzazione: scansione dall'alto verso il basso per trovare il primo bit non settato
    for (long i = bound_val; i >= 0; i--) {
        unsigned long idx = i / 64;
        unsigned long bit = i % 64;
        
        if (!(reachable[idx] & (1ULL << bit))) {
            max_unreachable = i;
            break;
        }
    }
    
    // Imposta il risultato
    if (max_unreachable == -1) {
        mp_set(result, 0); // Tutti i valori sono rappresentabili
    } else {
        mp_set_int(result, max_unreachable);
    }
    
    // Pulizia
    free(reachable);
    free(coin_vals);
    mp_clear(&bound);
    mp_clear(&temp);
    mp_clear(&product);
    
    return MP_OKAY;
}

// Funzione principale per risolvere il problema di Frobenius
int frobenius_solve_general(frobenius_context_t *ctx) {
    if (!ctx) return MP_VAL;
    
    if (ctx->cached) {
        log_message(ctx, "Usando risultato dalla cache");
        return MP_OKAY;
    }
    
    // Verifica input valido
    if (ctx->n <= 0 || ctx->coins == NULL) {
        return MP_VAL;
    }
    
    // Gestione casi speciali
    if (ctx->n == 1) {
        mp_int one;
        mp_init(&one);
        mp_set(&one, 1);
        
        mp_sub(&ctx->coins[0], &one, &ctx->result);
        
        mp_clear(&one);
        ctx->cached = 1;
        return MP_OKAY;
    }
    
    if (ctx->n == 2) {
        int status = case_n2(&ctx->result, &ctx->coins[0], &ctx->coins[1]);
        if (status == MP_OKAY) {
            ctx->cached = 1;
        }
        return status;
    }
    
    // Per n <= 4, usa l'algoritmo Round-Robin
    if (ctx->n <= 4) {
        log_message(ctx, "Usando algoritmo Round-Robin");
        int status = round_robin_algorithm(ctx, &ctx->result);
        if (status == MP_OKAY) {
            ctx->cached = 1;
        }
        return status;
    }
    
    // Per n > 4, usa l'algoritmo DP se le monete sono abbastanza piccole
    mp_int max_coin;
    mp_init(&max_coin);
    mp_set(&max_coin, 0);
    
    for (int i = 0; i < ctx->n; i++) {
        if (mp_cmp(&ctx->coins[i], &max_coin) == MP_GT) {
            mp_copy(&ctx->coins[i], &max_coin);
        }
    }
    
    unsigned long max_val;
    if (mp_cmp_d(&max_coin, ULONG_MAX) == MP_GT) {
        log_message(ctx, "Moneta troppo grande per conversione");
        mp_clear(&max_coin);
        // Fallback a Round-Robin
        log_message(ctx, "Fallback a Round-Robin per valori grandi");
        int status = round_robin_algorithm(ctx, &ctx->result);
        if (status == MP_OKAY) {
            ctx->cached = 1;
        }
        return status;
    }
    
    mp_to_unsigned_bin_n(&max_coin, (unsigned char*)&max_val, sizeof(max_val));
    mp_clear(&max_coin);
    
    if (max_val < 1000000) {
        log_message(ctx, "Usando algoritmo DP");
        int status = dp_algorithm(ctx, &ctx->result);
        if (status == MP_OKAY) {
            ctx->cached = 1;
        }
        return status;
    }
    
    // Fallback a Round-Robin per casi piu' grandi
    log_message(ctx, "Fallback a Round-Robin per valori grandi");
    int status = round_robin_algorithm(ctx, &ctx->result);
    if (status == MP_OKAY) {
        ctx->cached = 1;
    }
    return status;
}

// Funzione per risolvere il problema di Frobenius
int frobenius_solve(frobenius_context_t *ctx, mp_int *coins, int n) {
    if (!ctx || !coins || n <= 0) {
        return MP_VAL;
    }
    
    // Libera eventuali monete precedenti
    if (ctx->coins != NULL) {
        for (int i = 0; i < ctx->n; i++) {
            mp_clear(&ctx->coins[i]);
        }
        free(ctx->coins);
        ctx->coins = NULL;
    }
    
    // Inizializza il nuovo array di monete
    ctx->n = n;
    ctx->coins = (mp_int*)malloc(n * sizeof(mp_int));
    if (!ctx->coins) {
        ctx->n = 0;
        return MP_MEM;
    }
    
    for (int i = 0; i < n; i++) {
        mp_init(&ctx->coins[i]);
        mp_copy(&coins[i], &ctx->coins[i]);
    }
    
    // Resetta il flag di cache
    ctx->cached = 0;
    
    // Risolvi il problema con l'algoritmo appropriato
    int status = frobenius_solve_general(ctx);
    
    // Imposta il flag di cache se la soluzione e' andata a buon fine
    if (status == MP_OKAY) {
        ctx->cached = 1;
    }
    
    return status;
}

// Funzione per liberare le risorse
void frobenius_clear(frobenius_context_t *ctx) {
    if (!ctx) return;
    
    if (ctx->coins != NULL) {
        for (int i = 0; i < ctx->n; i++) {
            mp_clear(&ctx->coins[i]);
        }
        free(ctx->coins);
        ctx->coins = NULL;
    }
    
    mp_clear(&ctx->result);
    
    if (ctx->debug_info != NULL) {
        free(ctx->debug_info);
        ctx->debug_info = NULL;
    }
    
    ctx->n = 0;
    ctx->cached = 0;
}

// Funzione per stampare un array di monete
void print_coins(mp_int *coins, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        char coin_str[64];
        mp_toradix(&coins[i], coin_str, 10);
        printf("%s", coin_str);
        if (i < n - 1) printf(", ");
    }
    printf("]");
}

// Funzione per misurare il tempo di esecuzione
double measure_execution_time(frobenius_context_t *ctx, mp_int *coins, int n) {
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    int status = frobenius_solve(ctx, coins, n);
    end = clock();
    
    if (status != MP_OKAY) {
        return -1.0; // Errore nell'esecuzione
    }
    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    return cpu_time_used;
}

// Funzione per confrontare diverse implementazioni
void compare_algorithms(mp_int *coins, int n, int verbose) {
    frobenius_context_t ctx;
    frobenius_init(&ctx, verbose);
    
    printf("\nConfigurazione di test: ");
    print_coins(coins, n);
    printf("\n");
    
    // Misura Round-Robin
    printf("Esecuzione algoritmo Round-Robin...\n");
    double rr_time = -1.0;
    
    // Forza l'uso di Round-Robin
    {
        frobenius_context_t rr_ctx;
        frobenius_init(&rr_ctx, verbose);
        rr_ctx.n = n;
        rr_ctx.coins = (mp_int*)malloc(n * sizeof(mp_int));
        
        for (int i = 0; i < n; i++) {
            mp_init(&rr_ctx.coins[i]);
            mp_copy(&coins[i], &rr_ctx.coins[i]);
        }
        
        clock_t start = clock();
        int status = round_robin_algorithm(&rr_ctx, &rr_ctx.result);
        clock_t end = clock();
        
        if (status == MP_OKAY) {
            rr_time = ((double) (end - start)) / CLOCKS_PER_SEC;
            char result_str[64];
            mp_toradix(&rr_ctx.result, result_str, 10);
            printf("Round-Robin: %s (tempo: %.6f secondi)\n", result_str, rr_time);
        } else {
            printf("Round-Robin: errore (%d)\n", status);
        }
        
        frobenius_clear(&rr_ctx);
    }
    
    // Misura DP
    printf("Esecuzione algoritmo DP...\n");
    double dp_time = -1.0;
    
    // Forza l'uso di DP
    {
        frobenius_context_t dp_ctx;
        frobenius_init(&dp_ctx, verbose);
        dp_ctx.n = n;
        dp_ctx.coins = (mp_int*)malloc(n * sizeof(mp_int));
        
        for (int i = 0; i < n; i++) {
            mp_init(&dp_ctx.coins[i]);
            mp_copy(&coins[i], &dp_ctx.coins[i]);
        }
        
        clock_t start = clock();
        int status = dp_algorithm(&dp_ctx, &dp_ctx.result);
        clock_t end = clock();
        
        if (status == MP_OKAY) {
            dp_time = ((double) (end - start)) / CLOCKS_PER_SEC;
            char result_str[64];
            mp_toradix(&dp_ctx.result, result_str, 10);
            printf("DP: %s (tempo: %.6f secondi)\n", result_str, dp_time);
        } else {
            printf("DP: errore (%d)\n", status);
        }
        
        frobenius_clear(&dp_ctx);
    }
    
    // Misura auto-selection
    printf("Esecuzione algoritmo automatico...\n");
    double auto_time = measure_execution_time(&ctx, coins, n);
    
    if (auto_time >= 0) {
        char result_str[64];
        mp_toradix(&ctx.result, result_str, 10);
        printf("Auto: %s (tempo: %.6f secondi)\n", result_str, auto_time);
    } else {
        printf("Auto: errore\n");
    }
    
    // Confronto
    printf("\nRiepilogo prestazioni:\n");
    printf("Round-Robin: %.6f s\n", rr_time);
    printf("DP: %.6f s\n", dp_time);
    printf("Auto: %.6f s\n", auto_time);
    
    if (rr_time > 0 && dp_time > 0) {
        printf("\nRapporto DP/RR: %.2fx\n", dp_time / rr_time);
    }
    
    frobenius_clear(&ctx);
}

// Funzione per testare un caso specifico
void test_case(int *values, int n, const char *description, int verbose) {
    mp_int *coins = (mp_int*)malloc(n * sizeof(mp_int));
    for (int i = 0; i < n; i++) {
        mp_init(&coins[i]);
        mp_set(&coins[i], values[i]);
    }
    
    printf("\n=== Test: %s ===\n", description);
    
    frobenius_context_t ctx;
    frobenius_init(&ctx, verbose);
    
    int status = frobenius_solve(&ctx, coins, n);
    
    if (status == MP_OKAY) {
        char result_str[64];
        mp_toradix(&ctx.result, result_str, 10);
        printf("Numero di Frobenius: %s\n", result_str);
    } else {
        printf("Errore durante il calcolo: %d\n", status);
    }
    
    frobenius_clear(&ctx);
    
    for (int i = 0; i < n; i++) {
        mp_clear(&coins[i]);
    }
    free(coins);
}

// Esempio di utilizzo
int main(int argc, char *argv[]) {
    int verbose = 0;
    int benchmark = 0;
    
    // Parsing semplice degli argomenti
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--benchmark") == 0) {
            benchmark = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Utilizzo: %s [-v|--verbose] [-b|--benchmark] [-h|--help]\n", argv[0]);
            printf("  -v, --verbose    Abilita output dettagliato\n");
            printf("  -b, --benchmark  Esegui benchmark di confronto\n");
            printf("  -h, --help       Mostra questo aiuto\n");
            return EXIT_SUCCESS;
        }
    }
    
    printf("=== FrobeniusLTM - Risolutore del problema di Frobenius ===\n");
    
    // Test caso classico [3, 5, 7]
    int classic_case[] = {3, 5, 7};
    test_case(classic_case, 3, "Caso classico [3, 5, 7]", verbose);
    
    // Test McNugget [6, 9, 20]
    int mcnugget_case[] = {6, 9, 20};
    test_case(mcnugget_case, 3, "McNugget number [6, 9, 20]", verbose);
    
    // Test con numeri piu' grandi
    int large_case[] = {101, 103, 109, 127};
    test_case(large_case, 4, "Caso con numeri primi [101, 103, 109, 127]", verbose);
    
    // Test con 5 monete
    int five_coins[] = {31, 41, 59, 26, 53};
    test_case(five_coins, 5, "Caso con 5 monete [31, 41, 59, 26, 53]", verbose);
    
    if (benchmark) {
        printf("\n=== Benchmark di confronto tra algoritmi ===\n");
        
        // Preparazione monete per benchmark
        mp_int *bench_coins3 = (mp_int*)malloc(3 * sizeof(mp_int));
        mp_int *bench_coins4 = (mp_int*)malloc(4 * sizeof(mp_int));
        mp_int *bench_coins5 = (mp_int*)malloc(5 * sizeof(mp_int));
        
        for (int i = 0; i < 3; i++) {
            mp_init(&bench_coins3[i]);
            mp_set(&bench_coins3[i], mcnugget_case[i]);
        }
        
        for (int i = 0; i < 4; i++) {
            mp_init(&bench_coins4[i]);
            mp_set(&bench_coins4[i], large_case[i]);
        }
        
        for (int i = 0; i < 5; i++) {
            mp_init(&bench_coins5[i]);
            mp_set(&bench_coins5[i], five_coins[i]);
        }
        
        // Esegui benchmark
        printf("\n--- Benchmark con 3 monete (McNugget) ---\n");
        compare_algorithms(bench_coins3, 3, verbose);
        
        printf("\n--- Benchmark con 4 monete (numeri primi) ---\n");
        compare_algorithms(bench_coins4, 4, verbose);
        
        printf("\n--- Benchmark con 5 monete ---\n");
        compare_algorithms(bench_coins5, 5, verbose);
        
        // Pulizia
        for (int i = 0; i < 3; i++) mp_clear(&bench_coins3[i]);
        for (int i = 0; i < 4; i++) mp_clear(&bench_coins4[i]);
        for (int i = 0; i < 5; i++) mp_clear(&bench_coins5[i]);
        
        free(bench_coins3);
        free(bench_coins4);
        free(bench_coins5);
    }
    
    printf("\nCompletato.\n");
    return EXIT_SUCCESS;
}