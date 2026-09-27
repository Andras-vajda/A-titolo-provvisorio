/* Copyright (c) 1990-2026 M.A.W. 1968. SPDX-License-Identifier: MIT
 * Test white-box: compilare con sbm3.c e -DSBM3_TEST_HOOKS.
 * I controlli restano attivi con NDEBUG. Le sole alterazioni artificiali di
 * stato servono a collaudare overflow e confine terminale n=11 senza fingere
 * di aver percorso N(11) candidati. */
#include <stdio.h>
#include <stdlib.h>

#include "sbm3.h"

#define CHECK(c) do { if (!(c)) { \
    (void)fprintf(stderr, "FAIL linea %d: %s\n", __LINE__, #c); \
    exit(EXIT_FAILURE); } } while (0)

typedef struct {
    uint64_t bases;
    uint64_t matrices;
    bool pause;
} witness;

static uint64_t small_factorial(unsigned n)
{
    uint64_t value = 1U;
    unsigned i;
    for (i = 2U; i <= n; ++i) {
        value *= (uint64_t)i;
    }
    return value;
}

static bool observe(const sbm3_base *base, void *context)
{
    witness *w = context;
    unsigned i;
    unsigned j;
    CHECK(base->weight > 0U);
    for (j = 0U; j < base->n; ++j) {
        unsigned degree = 0U;
        for (i = 0U; i < base->n; ++i) {
            degree += (unsigned)((base->row[i] >> j) & UINT32_C(1));
        }
        CHECK(degree == 3U);
    }
    for (i = 0U; i < base->n; ++i) {
        unsigned degree = 0U;
        for (j = 0U; j < base->n; ++j) {
            degree += (unsigned)((base->row[i] >> j) & UINT32_C(1));
        }
        CHECK(degree == 3U);
        if (i > 0U) {
            CHECK(base->index[i - 1U] <= base->index[i]);
        }
    }
    ++w->bases;
    w->matrices += base->weight;
    return !w->pause;
}

static void same_counts(const sbm3_state *a, const sbm3_state *b)
{
    unsigned i;
    CHECK(a->finished == b->finished);
    CHECK(a->visited == b->visited);
    CHECK(a->bases == b->bases);
    CHECK(a->matrices == b->matrices);
    CHECK(a->simple_bases == b->simple_bases);
    CHECK(a->simple_matrices == b->simple_matrices);
    for (i = 0U; i < a->n; ++i) {
        CHECK(a->index[i] == b->index[i]);
    }
}

static void test_streams(void)
{
    static const uint64_t expected[] = {
        UINT64_C(1), UINT64_C(24), UINT64_C(2040),
        UINT64_C(297200), UINT64_C(68938800)
    };
    unsigned n;
    for (n = 3U; n <= 7U; ++n) {
        sbm3_state cached = { 0 };
        sbm3_state reference = { 0 };
        CHECK(sbm3_init(&cached, n, SBM3_CACHED) == SBM3_PAUSED);
        CHECK(sbm3_init(&reference, n, SBM3_REFERENCE) == SBM3_PAUSED);
        if (n <= 6U) {
            while (!cached.finished) {
                CHECK(sbm3_scan(&cached, 1U, NULL, NULL) ==
                      sbm3_scan(&reference, 1U, NULL, NULL));
                same_counts(&cached, &reference);
            }
        } else {
            CHECK(sbm3_scan(&cached, 0U, NULL, NULL) == SBM3_DONE);
            CHECK(sbm3_scan(&reference, 0U, NULL, NULL) == SBM3_DONE);
            same_counts(&cached, &reference);
        }
        CHECK(cached.visited == cached.candidate_count);
        CHECK(cached.matrices == expected[n - 3U]);
        CHECK(cached.simple_matrices == cached.simple_bases * small_factorial(n));
        CHECK(sbm3_scan(&cached, 1U, NULL, NULL) == SBM3_DONE);
        same_counts(&cached, &reference); /* Nessun doppio conteggio finale. */
    }
}

static void test_pauses(void)
{
    sbm3_state whole = { 0 };
    sbm3_state parts = { 0 };
    sbm3_state copy = { 0 };
    witness w = { 0U, 0U, true };
    unsigned batch = 1U;
    CHECK(sbm3_init(&whole, 5U, SBM3_CACHED) == SBM3_PAUSED);
    CHECK(sbm3_init(&parts, 5U, SBM3_CACHED) == SBM3_PAUSED);
    CHECK(sbm3_scan(&whole, 0U, NULL, NULL) == SBM3_DONE);
    while (!parts.finished) {
        const sbm3_status result = sbm3_scan(&parts, (uint64_t)batch, observe, &w);
        CHECK((result == SBM3_PAUSED) || (result == SBM3_DONE));
        CHECK(parts.matrices == w.matrices);
        CHECK(parts.bases == w.bases);
        batch = (batch % 37U) + 1U;
    }
    same_counts(&whole, &parts);
    CHECK(sbm3_init(&parts, 6U, SBM3_CACHED) == SBM3_PAUSED);
    CHECK(sbm3_scan(&parts, UINT64_C(12345), NULL, NULL) == SBM3_PAUSED);
    copy = parts;
    CHECK(sbm3_scan(&parts, 0U, NULL, NULL) == SBM3_DONE);
    CHECK(sbm3_scan(&copy, 0U, NULL, NULL) == SBM3_DONE);
    same_counts(&parts, &copy);
}

static uint32_t random_word(uint32_t *seed)
{
    *seed ^= *seed << 13U;
    *seed ^= *seed >> 17U;
    *seed ^= *seed << 5U;
    return *seed;
}

static void test_cache_and_boundaries(void)
{
    uint32_t seed = UINT32_C(0x51b3a271);
    unsigned n;
    for (n = 3U; n <= 11U; ++n) {
        sbm3_state initial = { 0 };
        unsigned trial;
        CHECK(sbm3_init(&initial, n, SBM3_CACHED) == SBM3_PAUSED);
        for (trial = 0U; trial < 500U; ++trial) {
            sbm3_state s = initial;
            unsigned i;
            unsigned step;
            for (i = 0U; i < n; ++i) {
                const uint16_t q = (uint16_t)(random_word(&seed) % s.pattern_count);
                unsigned j = i;
                while ((j > 0U) && (s.index[j - 1U] > q)) {
                    s.index[j] = s.index[j - 1U];
                    --j;
                }
                s.index[j] = q;
            }
            for (step = 0U; (step < 64U) && !s.finished; ++step) {
                const bool expected = sbm3_test_valid_reference(&s);
                CHECK(sbm3_test_valid_cached(&s) == expected);
                sbm3_test_advance(&s);
            }
        }
        {   /* Base ciclica valida anche al confine superiore n=11. */
            sbm3_state s = initial;
            unsigned i;
            for (i = 0U; i < n; ++i) {
                const uint32_t mask = (UINT32_C(1) << i) |
                    (UINT32_C(1) << ((i + 1U) % n)) |
                    (UINT32_C(1) << ((i + 2U) % n));
                unsigned q = 0U;
                unsigned j = i;
                while ((q < s.pattern_count) && (s.pattern[q] != mask)) {
                    ++q;
                }
                CHECK(q < s.pattern_count);
                while ((j > 0U) && ((unsigned)s.index[j - 1U] > q)) {
                    s.index[j] = s.index[j - 1U];
                    --j;
                }
                s.index[j] = (uint16_t)q;
            }
            CHECK(sbm3_test_valid_reference(&s));
            CHECK(sbm3_test_valid_cached(&s));
            CHECK(sbm3_test_weight(&s) == ((n == 3U) ? UINT64_C(1) : small_factorial(n)));
        }
        {   /* Ultimo scatto, senza affermare una scansione completa a n=11. */
            sbm3_state s = initial;
            unsigned i;
            for (i = 0U; i < n; ++i) {
                s.index[i] = (uint16_t)(s.pattern_count - 1U);
            }
            CHECK(sbm3_test_weight(&s) == 1U);
            sbm3_test_advance(&s);
            CHECK(s.finished);
        }
    }
}

static void test_errors(void)
{
    sbm3_state s = { 0 };
    witness w = { 0U, 0U, false };
    CHECK(sbm3_init(NULL, 3U, SBM3_CACHED) == SBM3_BAD_ARGUMENT);
    CHECK(sbm3_scan(NULL, 0U, NULL, NULL) == SBM3_BAD_ARGUMENT);
    CHECK(sbm3_init(&s, 0U, SBM3_CACHED) == SBM3_BAD_ARGUMENT);
    CHECK(sbm3_scan(&s, 0U, NULL, NULL) == SBM3_BAD_ARGUMENT);
    CHECK(sbm3_init(&s, 12U, SBM3_CACHED) == SBM3_BAD_ARGUMENT);
    CHECK(sbm3_init(&s, 3U, (sbm3_engine)99) == SBM3_BAD_ARGUMENT);
    CHECK(sbm3_init(&s, 3U, SBM3_CACHED) == SBM3_PAUSED);
    s.matrices = UINT64_MAX; /* Fault injection: guardia realmente attiva. */
    CHECK(sbm3_scan(&s, 0U, observe, &w) == SBM3_OVERFLOW);
    CHECK((s.visited == 0U) && (w.bases == 0U) && !s.finished);
    s.matrices = 0U;
    CHECK(sbm3_scan(&s, 0U, observe, &w) == SBM3_DONE);
    CHECK((s.matrices == 1U) && (w.bases == 1U));
    CHECK(sbm3_init(&s, 4U, SBM3_CACHED) == SBM3_PAUSED);
    s.index[0] = 0U; s.index[1] = 1U; s.index[2] = 2U; s.index[3] = 3U;
    s.simple_matrices = UINT64_MAX;
    CHECK(sbm3_scan(&s, 1U, NULL, NULL) == SBM3_OVERFLOW);
    CHECK((s.visited == 0U) && (s.matrices == 0U));
    CHECK(sbm3_init(&s, 11U, SBM3_CACHED) == SBM3_PAUSED);
    CHECK(s.candidate_count == UINT64_C(85695033024571425));
}

int main(void)
{
    test_streams();
    test_pauses();
    test_cache_and_boundaries();
    test_errors();
    (void)puts("OK: scansioni n=3..7; confronto candidato per candidato n<=6;");
    (void)puts("pause, callback, copia di stato, cache e confini fino a n=11;");
    (void)puts("overflow esplicito verificato mediante fault injection.");
    return EXIT_SUCCESS;
}
