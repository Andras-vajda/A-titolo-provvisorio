/* Copyright (c) 1990-2026 M.A.W. 1968. SPDX-License-Identifier: MIT */
#include "sbm3.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    bool matrices;
    bool simple_only;
    bool io_error;
    uint64_t emitted;
} output_context;

static void usage(FILE *out)
{
    (void)fputs("Uso: sbm3 [n] [--reference] [--limit N]\n"
                "            [--emit-bases | --emit-matrices] [--simple]\n"
                "n: 3..11, predefinito 6. Nessun argomento richiede memoria dinamica.\n"
                "--limit N: visita al massimo N candidati (0 = tutti).\n"
                "--reference: usa il controllo con vettore dei gradi.\n"
                "--emit-bases: una riga B peso riga1 ... rigan per quadrato base.\n"
                "--emit-matrices: una riga M riga1 ... rigan per matrice ordinata.\n"
                "--simple: emette solo risultati senza righe ripetute.\n"
                "Con emissione, il riepilogo va su stderr; i dati vanno su stdout.\n"
                "Il limite n=11 e' aritmetico: non promette tempi pratici.\n", out);
}

/* Parsing decimale totale: niente segni, spazi, troncamenti o wraparound. */
static bool decimal(const char *text, uint64_t *value)
{
    uint64_t result = 0U;
    const char *p = text;
    if (*p == '\0') {
        return false;
    }
    while (*p != '\0') {
        uint64_t digit;
        if ((*p < '0') || (*p > '9')) {
            return false;
        }
        digit = (uint64_t)(*p - '0');
        if (result > (UINT64_MAX - digit) / UINT64_C(10)) {
            return false;
        }
        result = result * UINT64_C(10) + digit;
        ++p;
    }
    *value = result;
    return true;
}

/* Successore lessicografico delle permutazioni distinte di maschere.
 * Le uguaglianze rendono superfluo qualsiasi insieme di deduplicazione. */
static bool next_permutation(uint32_t a[SBM3_MAX_N], unsigned n)
{
    unsigned i = n - 1U;
    unsigned j;
    uint32_t tmp;
    while ((i > 0U) && (a[i - 1U] >= a[i])) {
        --i;
    }
    if (i == 0U) {
        return false;
    }
    j = n - 1U;
    while (a[j] <= a[i - 1U]) {
        --j;
    }
    tmp = a[i - 1U];
    a[i - 1U] = a[j];
    a[j] = tmp;
    j = n - 1U;
    while (i < j) {
        tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
        ++i;
        --j;
    }
    return true;
}

static bool print_rows(const uint32_t row[SBM3_MAX_N], unsigned n)
{
    unsigned i;
    unsigned j;
    for (i = 0U; i < n; ++i) {
        if (putchar(' ') == EOF) {
            return false;
        }
        for (j = 0U; j < n; ++j) {
            const int bit = ((row[i] & (UINT32_C(1) << j)) != 0U) ? '1' : '0';
            if (putchar(bit) == EOF) {
                return false;
            }
        }
    }
    return putchar('\n') != EOF;
}

static bool emit(const sbm3_base *base, void *context)
{
    output_context *out = context;
    unsigned i;
    if (out->simple_only) {
        for (i = 1U; i < base->n; ++i) {
            if (base->index[i] == base->index[i - 1U]) {
                return true;
            }
        }
    }
    if (!out->matrices) {
        if ((printf("B %" PRIu64, base->weight) < 0) ||
            !print_rows(base->row, base->n)) {
            out->io_error = true;
            return false;
        }
        ++out->emitted;
    } else {
        uint32_t row[SBM3_MAX_N];
        /* Ordine numerico delle maschere, solo per espandere l'orbita. */
        for (i = 0U; i < base->n; ++i) {
            unsigned j = i;
            while ((j > 0U) && (row[j - 1U] > base->row[i])) {
                row[j] = row[j - 1U];
                --j;
            }
            row[j] = base->row[i];
        }
        do {
            if ((putchar('M') == EOF) || !print_rows(row, base->n)) {
                out->io_error = true;
                return false;
            }
            ++out->emitted;
        } while (next_permutation(row, base->n));
    }
    return true;
}

int main(int argc, char *argv[])
{
    sbm3_state state;
    sbm3_engine engine = SBM3_CACHED;
    sbm3_status status;
    output_context output = { false, false, false, 0U };
    uint64_t limit = 0U;
    unsigned n = 6U;
    bool have_n = false;
    bool emit_requested = false;
    bool have_limit = false;
    FILE *report;
    clock_t begin;
    clock_t end;
    int arg;
    for (arg = 1; arg < argc; ++arg) {
        if (strcmp(argv[arg], "--help") == 0) {
            usage(stdout);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[arg], "--reference") == 0) {
            engine = SBM3_REFERENCE;
        } else if (strcmp(argv[arg], "--simple") == 0) {
            output.simple_only = true;
        } else if ((strcmp(argv[arg], "--emit-bases") == 0) ||
                   (strcmp(argv[arg], "--emit-matrices") == 0)) {
            if (emit_requested) {
                usage(stderr);
                return EXIT_FAILURE;
            }
            emit_requested = true;
            output.matrices = strcmp(argv[arg], "--emit-matrices") == 0;
        } else if (strcmp(argv[arg], "--limit") == 0) {
            ++arg;
            if (have_limit || (arg >= argc) || !decimal(argv[arg], &limit)) {
                usage(stderr);
                return EXIT_FAILURE;
            }
            have_limit = true;
        } else {
            uint64_t parsed;
            if (have_n || !decimal(argv[arg], &parsed) ||
                (parsed < (uint64_t)SBM3_MIN_N) ||
                (parsed > (uint64_t)SBM3_MAX_N)) {
                (void)fputs("Argomento non valido; n deve essere compreso fra 3 e 11.\n", stderr);
                usage(stderr);
                return EXIT_FAILURE;
            }
            n = (unsigned)parsed;
            have_n = true;
        }
    }
    if (output.simple_only && !emit_requested) {
        (void)fputs("--simple filtra l'emissione; i contatori semplici sono sempre riportati.\n", stderr);
        return EXIT_FAILURE;
    }
    status = sbm3_init(&state, n, engine);
    if (status != SBM3_PAUSED) {
        (void)fputs("Inizializzazione fallita.\n", stderr);
        return EXIT_FAILURE;
    }
    begin = clock();
    status = sbm3_scan(&state, limit, emit_requested ? emit : NULL, &output);
    end = clock();
    if ((status == SBM3_OVERFLOW) || (status == SBM3_BAD_ARGUMENT)) {
        (void)fputs("Errore di enumerazione: overflow o stato non inizializzato.\n", stderr);
        return EXIT_FAILURE;
    }
    if (output.io_error || (fflush(stdout) == EOF)) {
        (void)fputs("Errore di scrittura dei risultati.\n", stderr);
        return EXIT_FAILURE;
    }
    report = emit_requested ? stderr : stdout;
    (void)fprintf(report,
        "status=%s\nn=%u\nengine=%s\npatterns=%u\n"
        "expected_candidates=%" PRIu64 "\nvisited=%" PRIu64 "\n"
        "bases_counted=%" PRIu64 "\nmatrices_counted=%" PRIu64 "\n"
        "simple_bases_counted=%" PRIu64 "\nsimple_matrices_counted=%" PRIu64 "\n",
        state.finished ? "complete" : "PARTIAL", state.n,
        (engine == SBM3_CACHED) ? "cached" : "reference", state.pattern_count,
        state.candidate_count, state.visited, state.bases, state.matrices,
        state.simple_bases, state.simple_matrices);
    if (emit_requested) {
        (void)fprintf(report, "emitted=%" PRIu64 "\n", output.emitted);
    }
    if ((begin != (clock_t)-1) && (end != (clock_t)-1) && (end >= begin)) {
        (void)fprintf(report, "cpu_seconds=%.6f\n", (double)(end - begin) / (double)CLOCKS_PER_SEC);
    }
    if (!state.finished) {
        (void)fputs("Scansione parziale: i contatori non sono il totale f(n).\n", report);
    }
    return (fflush(report) == EOF) ? EXIT_FAILURE : EXIT_SUCCESS;
}
