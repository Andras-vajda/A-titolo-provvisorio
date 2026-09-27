/* Copyright (c) 1990-2026 M.A.W. 1968. SPDX-License-Identifier: MIT */
#ifndef SBM3_H
#define SBM3_H

#include <stdbool.h>
#include <stdint.h>

/* Limiti di progetto, non opzioni modificabili con -D. */
enum { SBM3_MIN_N = 3, SBM3_MAX_N = 11, SBM3_MAX_PATTERNS = 165 };

typedef enum {
    SBM3_CACHED = 0,
    SBM3_REFERENCE = 1
} sbm3_engine;

typedef enum {
    SBM3_DONE = 0,
    SBM3_PAUSED = 1,
    SBM3_BAD_ARGUMENT = 2,
    SBM3_OVERFLOW = 3
} sbm3_status;

typedef struct {
    unsigned n;
    uint16_t index[SBM3_MAX_N];
    uint32_t row[SBM3_MAX_N]; /* Il bit j rappresenta la colonna j+1. */
    uint64_t weight;
} sbm3_base;

/* Il puntatore base vale soltanto durante la chiamata. Restituire false
 * chiede una pausa DOPO il candidato corrente, che viene contato una volta.
 * Il callback non deve modificare o riavviare lo stesso enumeratore. */
typedef bool (*sbm3_visitor)(const sbm3_base *base, void *context);

/* Memoria posseduta dal chiamante: niente heap, VLA o stato globale mutabile.
 * I campi sono consultabili; modificarli fuori da init/scan viola il contratto.
 * Tra due chiamate, index e' il primo candidato non ancora visitato, oppure
 * l'ultimo candidato se finished e' true. I contatori coprono il prefisso
 * gia' esaminato, esattamente come nell'invariante della specifica Z.
 * La struttura puo' essere copiata per creare due scansioni indipendenti. */
typedef struct {
    unsigned n;
    unsigned pattern_count;
    sbm3_engine engine;
    bool initialized;
    bool finished;
    uint32_t all_columns;
    uint32_t pattern[SBM3_MAX_PATTERNS];
    uint16_t index[SBM3_MAX_N];
    uint32_t low[SBM3_MAX_N + 1];
    uint32_t high[SBM3_MAX_N + 1];
    unsigned valid_depth;
    unsigned dirty;
    uint64_t candidate_count;
    uint64_t visited;
    uint64_t bases;
    uint64_t matrices;
    uint64_t simple_bases;
    uint64_t simple_matrices;
} sbm3_state;

/* Accetta soltanto 3 <= n <= 11. Su errore, uno stato non nullo resta
 * non inizializzato e non puo' essere usato per una scansione. */
sbm3_status sbm3_init(sbm3_state *state, unsigned n, sbm3_engine engine);

/* Visita al piu' budget candidati; budget=0 significa fino alla fine.
 * visitor=NULL abilita il solo conteggio. Una pausa e' riprendibile con
 * un'altra scan: non si ripete e non si perde il candidato di confine.
 * SBM3_DONE significa scansione completa; SBM3_PAUSED significa prefisso.
 * Su overflow il candidato corrente non viene contato ne' notificato.
 * state deve essere stato passato a sbm3_init prima di questa funzione. */
sbm3_status sbm3_scan(sbm3_state *state, uint64_t budget,
                      sbm3_visitor visitor, void *context);

/* Solo per il collaudo white-box in una translation unit separata.
 * Questi simboli non esistono nella compilazione di produzione. */
#ifdef SBM3_TEST_HOOKS
bool sbm3_test_valid_reference(const sbm3_state *state);
bool sbm3_test_valid_cached(sbm3_state *state);
uint64_t sbm3_test_weight(const sbm3_state *state);
void sbm3_test_advance(sbm3_state *state);
#endif

#endif
