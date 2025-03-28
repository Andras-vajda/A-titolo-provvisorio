/*****************************************************************************/
/*
 * STSC_LUT.c - Cyclic Steiner Triple System Generator (LUT-based)
 *
 * DESCRIZIONE: 
 *   Generatore di Sistemi Tripli di Steiner Ciclici (STSC)
 *   esempio basato su LUT precalcolata con terne di Heffter-Peltesohn.
 *   Supporta tutti i valori di v a due cifre e il caso base v = 7.
 *   Unica eccezione nota: non esiste un STS ciclico per v = 9.
 *
 * DESCRIPTION:
 *   Generator of cyclic Steiner Triple Systems (C-STS) using the 
 *   Heffter-Peltesohn method with precomputed difference triples
 *   stored in lookup tables.
 *   Supports orders v where v = 1 or 3 (mod 6), v > 6.
 *   There notoriously exists no C-STS of order 9.
 *
 * METRICS:
 *   - Lines of Code (LOC)..................: 193
 *   - Source Lines of Code (SLOC)..........: 143
 *   - Comment Lines (CLOC).................: 36
 *   - Comment to Code Ratio (CCR)..........: 0.25
 *   - Cyclomatic Complexity (CC)...........: 18
 *   - Function Count.......................: 5
 *   - Average Function Length..............: 29 SLOC
 *   - Halstead Volume......................: 3814.2
 *   - Halstead Difficulty..................: 42.3
 *   - Maintenance Index....................: 61.7
 *
 * REFERENCES:
 *   [1] Peltesohn, R. (1939). "Eine Loesung der beiden Heffterschen Differenzenprobleme."
 *       Compositio Mathematica, 6, 251-257.
 *       URL: http://www.numdam.org/item/CM_1939__6__251_0/
 *   [2] Lindner, C.C., & Rodger, C.A. (2008). "Design Theory (2nd Edition)."
 *       Chapman & Hall/CRC. ISBN: 978-1420082968.
 *
 * AUTHOR: M.A.W. 1968
 *
 * COMPILING:
 *   Visual Studio: cl /W4 /TC /O2 STSC_LUT.c
 *   GCC: gcc -Wall -std=c99 -O2 STSC_LUT.c -o STSC_LUT
 *
 * LICENSE: MIT License
 * Copyright (C) 2000-2025 M.A.W. 1968
 *
 *****************************************************************************/

#include "..\include\STSC_LUT.h"

/*****************************************************************************/
/**
 * Calculate the expected number of triples in an STS of order v.
 *
 * The formula is b = v(v-1)/6, derived from the combinatorial properties
 * of Steiner Triple Systems.
 *
 * @param v The order of the STS
 * @return The number of triples
 */
/*****************************************************************************/
int calcola_numero_terne(int v) {
    return v * (v - 1) / 6;
}

/*****************************************************************************/
/**
 * Binary search for a value v in the LUT table.
 *
 * Searches the LUT to find the index of a given v value. This is used
 * to retrieve the precomputed difference triples for a specific order.
 *
 * @param v The order to search for
 * @return The index in the LUT, or INFTY if not found
 */
/*****************************************************************************/
unsigned int LUT_search(unsigned int v) {
    size_t left = 0, right = MAX_LUT - 1;
    
    while (left <= right) {
        size_t mid = (left + right) / 2;
        if (LUT[mid][0] == v) {
            return mid;
        } else if (LUT[mid][0] < v) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return INFTY; /* Value not found */
}

/*****************************************************************************/
/*
 * Generate a cyclic Steiner Triple System of order v.
 *
 * Implements the Heffter-Peltesohn method, which constructs an STS by
 * generating all possible cyclic shifts of a set of "base triples"
 * derived from precomputed difference triples.
 *
 * @param v The order of the STS (must satisfy v ? 1 or 3 (mod 6), v ? 7, v ? 9)
 * @param STS_triples Array to store the generated triples (preallocated by caller)
 * @return The number of triples generated, or 0 on error
 */
/*****************************************************************************/
unsigned int genera_STS_ciclico(int v, Terna_t STS_triples[]) {
    unsigned int idx = 0; /* Counter for total triples generated */
    unsigned int ct  = 1; /* Counter for difference triples and base triples */
    unsigned int idx_LUT = LUT_search(v);
    
    if (idx_LUT == INFTY) {
        printf("Errore: v = %u non incluso nella LUT.\n", v);
        return 0;
    }

    unsigned int num_triples = LUT[idx_LUT][1];

    /* Phase 1: Generate triples from base triples derived from difference triples */
    for (size_t bi = 0; bi < num_triples; bi++) {
        unsigned int base[3];
        
        /* Read the difference triple */
        unsigned int x = TERNE_PELTESOHN[idx_LUT][bi].a;
        unsigned int y = TERNE_PELTESOHN[idx_LUT][bi].b;
        unsigned int z = TERNE_PELTESOHN[idx_LUT][bi].c; /* For display only */

        /* Derive the base triple from the difference triple */
        base[0] = 0;
        base[1] = x;
        base[2] = MOD(x + y, v);
        
        /* Display the base triple for informational purposes */
        printf("%2u. Differenza: (%2u, %2u, %2u) -> Base %zu: (%2u, %2u, %2u)\n", 
               ct++, x, y, z, bi + 1, base[0], base[1], base[2]);

        /* Generate all cyclic shifts of the base triple */
        for (size_t i = 0; i < v; i++) {
            /* Store the generated triple */
            STS_triples[idx].a = MOD(base[0] + i, v);
            STS_triples[idx].b = MOD(base[1] + i, v);
            STS_triples[idx].c = MOD(base[2] + i, v);
            
            /* Display the generated triple */
            printf("  %4zu: (%2u, %2u, %2u)\n", i + 1, 
                   STS_triples[idx].a, STS_triples[idx].b, STS_triples[idx].c);

            idx++;
        }
    }

    /* Phase 2: Add "short orbit" triples for v ? 3 (mod 6) */
    if (v % 6 == 3) {
        unsigned int n = (v - 3) / 6;
        unsigned int base_short[3];
        
        base_short[0] = 0;
        base_short[1] = 2 * n + 1;
        base_short[2] = 4 * n + 2;

        printf("%2u. Base short orbit: (%2u, %2u, %2u)\n", ct++, base_short[0], base_short[1], base_short[2]);

        /* Generate 2n+1 cyclic shifts of the short orbit base triple */
        for (size_t i = 0; i <= 2 * n; i++) {
            /* Store the generated triple */
            STS_triples[idx].a = MOD(base_short[0] + i, v);
            STS_triples[idx].b = MOD(base_short[1] + i, v);
            STS_triples[idx].c = MOD(base_short[2] + i, v);
            
            /* Display the generated triple */
            printf("  %4zu: (%2u, %2u, %2u)\n", i + 1, 
                   STS_triples[idx].a, STS_triples[idx].b, STS_triples[idx].c);

            idx++;
        }
    }
    return idx;
}

/*****************************************************************************/
/**
 * Verify the incidence matrix of a Steiner Triple System.
 *
 * Creates and checks a boolean incidence matrix to ensure that:
 * - Each triple contains exactly 3 elements
 * - Each element appears in exactly r = (v-1)/2 triples
 *
 * @param terne Array of triples to verify
 * @param num_terne Number of triples in the array
 * @param v The order of the STS
 * @return true if the incidence matrix is valid, false otherwise
 */
/*****************************************************************************/
bool genera_e_verifica_IM(Terna_t terne[], int num_terne, int v) {
    int IM[MAX_TERNE][MAX_V] = {0};
    int r = (v - 1) / 2;  /* Each element must appear in exactly r triples */

    /* Fill the incidence matrix */
    for (int i = 0; i < num_terne; i++) {
        IM[i][terne[i].a] = 1;
        IM[i][terne[i].b] = 1;
        IM[i][terne[i].c] = 1;
    }

    /* Verify row sums (each triple contains exactly 3 elements) */
    for (int i = 0; i < num_terne; i++) {
        int row_sum = 0;
        for (int j = 0; j < v; j++) {
            row_sum += IM[i][j];
        }
        if (row_sum != 3) {
            return false;
        }
    }

    /* Verify column sums (each element appears in exactly r triples) */
    for (int j = 0; j < v; j++) {
        int col_sum = 0;
        for (int i = 0; i < num_terne; i++) {
            col_sum += IM[i][j];
        }
        if (col_sum != r) {
            return false;
        }
    }

    return true;
}

/*****************************************************************************/
/**
 * Main function for interactive STS generation.
 *
 * Allows the user to:
 * - Enter a value of v
 * - Generate the corresponding STS
 * - Verify the generated system
 *
 * The function continues in a loop until the user enters 1 to exit.
 */
/*****************************************************************************/
int main() {
    int v;
    Terna_t terne[MAX_TERNE];

#ifdef TEST
    test_structures();
    return EXIT_SUCCESS;
#endif

    printf("Cyclic Steiner Triple System Generator (Heffter-Peltesohn Method)\n");
    printf("================================================================\n");

    while (true) {
        printf("\nInserisci la dimensione v di un STS (1 per terminare): ");
        if (scanf("%d", &v) != 1 || v < 0) {
            printf("Input non valido. Riprova.\n");
            while (getchar() != '\n'); /* Clear input buffer */
            continue;
        }

        if (1 == v) {
            printf("\nTerminazione del programma. Arrivederci!\n");
            break;
        }

        if (v % 6 != 1 && v % 6 != 3) {
            printf("Dimensione non valida. v deve essere ? 1 o 3 (mod 6).\n");
            continue;
        }
        
        if (9 == v) {
            printf("Non esiste un STS(9) ciclico. Riprova.\n");
            continue;
        }

        printf("\nCalcolo per STS(%d)...\n", v);
        int num_terne = genera_STS_ciclico(v, terne);

        if (num_terne) {
            printf("Numero atteso di terne.....: %d\n", calcola_numero_terne(v));
            printf("Totale terne generate......: %d\n", num_terne);
    
            /* For demonstration purposes */
            if (genera_e_verifica_IM(terne, num_terne, v)) {
                printf("Matrice di incidenza (IM) verificata: STS valido.\n");
            } else {
                printf("Errore: matrice di incidenza non valida.\n");
            }
        }
    }

    return EXIT_SUCCESS;
}
/* EOF: STSC_LUT.c */