/* Copyright (c) 1990-2026 M.A.W. 1968. SPDX-License-Identifier: MIT */
#include "sbm3.h"

#include <stddef.h>
#include <string.h>

_Static_assert(SBM3_MAX_N == 11, "Riesaminare tutte le prove prima di alzare n");
_Static_assert(SBM3_MAX_PATTERNS ==
               SBM3_MAX_N * (SBM3_MAX_N - 1) * (SBM3_MAX_N - 2) / 6,
               "Catalogo incoerente con il limite");
_Static_assert(SBM3_MAX_N < 32, "Le maschere devono entrare in uint32_t");

static const uint64_t factorial[SBM3_MAX_N + 1] = {
    UINT64_C(1), UINT64_C(1), UINT64_C(2), UINT64_C(6),
    UINT64_C(24), UINT64_C(120), UINT64_C(720), UINT64_C(5040),
    UINT64_C(40320), UINT64_C(362880), UINT64_C(3628800), UINT64_C(39916800)
};

/* Precondizione value != 0, garantita dal while del chiamante.
 * Equivalente C11 puro di ctz32: nessun intrinseco e nessun caso ctz(0). */
static unsigned low_bit_index(uint32_t value)
{
    unsigned j = 0U;
    while ((value & UINT32_C(1)) == 0U) {
        value >>= 1U;
        ++j;
    }
    return j;
}

/* Traduzione diretta del controllo di margini del progetto C11. */
static bool valid_reference(const sbm3_state *s)
{
    unsigned degree[SBM3_MAX_N] = { 0U };
    unsigned i;
    unsigned j;
    for (i = 0U; i < s->n; ++i) {
        uint32_t mask = s->pattern[s->index[i]];
        while (mask != 0U) {
            j = low_bit_index(mask);
            ++degree[j];
            if (degree[j] > 3U) {
                return false;
            }
            mask &= mask - UINT32_C(1);
        }
    }
    for (j = 0U; j < s->n; ++j) {
        if (degree[j] != 3U) {
            return false;
        }
    }
    return true;
}

/* low[k], high[k] codificano i gradi del prefisso memorizzato:
 * grado(j) = bit_j(low[k]) + 2 * bit_j(high[k]), quando k <= valid_depth.
 * dirty e' la prima posizione modificata dall'ultimo scatto (zero-based).
 * Per il NUOVO candidato si riusano soltanto i prefissi k <= dirty.
 * Se dirty > valid_depth, la precedente riga che causava overflow non e'
 * cambiata: lo stesso prefisso basta a rifiutare QUESTO candidato.
 * L'odometro avanza comunque di un solo candidato; nessun sottoalbero salta. */
static bool valid_cached(sbm3_state *s)
{
    unsigned i;
    uint32_t low;
    uint32_t high;
    if (s->dirty > s->valid_depth) {
        return false;
    }
    low = s->low[s->dirty];
    high = s->high[s->dirty];
    for (i = s->dirty; i < s->n; ++i) {
        const uint32_t mask = s->pattern[s->index[i]];
        if ((low & high & mask) != 0U) {
            s->valid_depth = i;
            return false;
        }
        /* Il riporto usa il vecchio low; l'ordine delle due righe conta. */
        high ^= low & mask;
        low ^= mask;
        s->low[i + 1U] = low;
        s->high[i + 1U] = high;
    }
    s->valid_depth = s->n;
    return (low & high) == s->all_columns;
}

/* Il prodotto dei fattoriali dei run divide n! ed e' <= n!.
 * A n <= 11 anche i prodotti intermedi entrano in uint64_t. */
static uint64_t base_weight(const sbm3_state *s)
{
    uint64_t denominator = UINT64_C(1);
    unsigned length = 1U;
    unsigned i;
    for (i = 1U; i < s->n; ++i) {
        if (s->index[i] == s->index[i - 1U]) {
            ++length;
        } else {
            denominator *= factorial[length];
            length = 1U;
        }
    }
    denominator *= factorial[length];
    return factorial[s->n] / denominator;
}

/* NextContinue / NextHalt, dopo l'accumulo del candidato corrente. */
static void advance(sbm3_state *s)
{
    unsigned end = s->n;
    unsigned i;
    uint16_t next;
    while ((end > 0U) &&
           ((unsigned)s->index[end - 1U] == s->pattern_count - 1U)) {
        --end;
    }
    if (end == 0U) {
        s->finished = true;
    } else {
        s->dirty = end - 1U;
        next = (uint16_t)((unsigned)s->index[s->dirty] + 1U);
        for (i = s->dirty; i < s->n; ++i) {
            s->index[i] = next;
        }
    }
}

sbm3_status sbm3_init(sbm3_state *s, unsigned n, sbm3_engine engine)
{
    unsigned i;
    unsigned j;
    unsigned k;
    uint64_t candidates = UINT64_C(1);
    if (s == NULL) {
        return SBM3_BAD_ARGUMENT;
    }
    (void)memset(s, 0, sizeof(*s));
    if ((n < (unsigned)SBM3_MIN_N) || (n > (unsigned)SBM3_MAX_N) ||
        ((engine != SBM3_CACHED) && (engine != SBM3_REFERENCE))) {
        return SBM3_BAD_ARGUMENT;
    }
    s->n = n;
    s->engine = engine;
    s->all_columns = (UINT32_C(1) << n) - UINT32_C(1);
    for (i = 0U; i + 2U < n; ++i) {
        for (j = i + 1U; j + 1U < n; ++j) {
            for (k = j + 1U; k < n; ++k) {
                s->pattern[s->pattern_count] =
                    (UINT32_C(1) << i) | (UINT32_C(1) << j) |
                    (UINT32_C(1) << k);
                ++s->pattern_count;
            }
        }
    }
    /* N(n)=binom(C+n-1,n). Ogni divisione e' esatta.
     * Il massimo prodotto intermedio e' 11*N(11) < UINT64_MAX.
     * La guardia resta attiva, indipendentemente da NDEBUG. */
    for (i = 1U; i <= n; ++i) {
        const uint64_t factor = (uint64_t)s->pattern_count + (uint64_t)i - 1U;
        if (candidates > UINT64_MAX / factor) {
            return SBM3_OVERFLOW;
        }
        candidates = (candidates * factor) / (uint64_t)i;
    }
    s->candidate_count = candidates;
    s->initialized = true;
    return SBM3_PAUSED;
}

sbm3_status sbm3_scan(sbm3_state *s, uint64_t budget,
                      sbm3_visitor visitor, void *context)
{
    const bool bounded = budget != 0U;
    if ((s == NULL) || !s->initialized) {
        return SBM3_BAD_ARGUMENT;
    }
    if (s->finished) {
        return SBM3_DONE;
    }
    while (!s->finished) {
        bool keep_going = true;
        const bool valid = (s->engine == SBM3_CACHED) ?
                          valid_cached(s) : valid_reference(s);
        if (valid) {
            const uint64_t weight = base_weight(s);
            const bool simple = weight == factorial[s->n];
            if ((s->matrices > UINT64_MAX - weight) ||
                (simple && (s->simple_matrices > UINT64_MAX - weight))) {
                return SBM3_OVERFLOW;
            }
            s->matrices += weight;
            ++s->bases;
            if (simple) {
                s->simple_matrices += weight;
                ++s->simple_bases;
            }
            if (visitor != NULL) {
                sbm3_base base;
                unsigned i;
                base.n = s->n;
                base.weight = weight;
                for (i = 0U; i < s->n; ++i) {
                    base.index[i] = s->index[i];
                    base.row[i] = s->pattern[s->index[i]];
                }
                keep_going = visitor(&base, context);
            }
        }
        /* Questi contatori sono <= N(11) < UINT64_MAX, anche su prefissi. */
        ++s->visited;
        advance(s);
        if (s->finished) {
            return SBM3_DONE;
        }
        if (!keep_going) {
            return SBM3_PAUSED;
        }
        if (bounded) {
            --budget;
            if (budget == 0U) {
                return SBM3_PAUSED;
            }
        }
    }
    return SBM3_DONE;
}

#ifdef SBM3_TEST_HOOKS
bool sbm3_test_valid_reference(const sbm3_state *s)
{
    return valid_reference(s);
}

bool sbm3_test_valid_cached(sbm3_state *s)
{
    return valid_cached(s);
}

uint64_t sbm3_test_weight(const sbm3_state *s)
{
    return base_weight(s);
}

void sbm3_test_advance(sbm3_state *s)
{
    advance(s);
}
#endif
