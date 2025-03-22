#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_N 64 // Massima cardinalità dell'universo

void printSubset(uint64_t B, size_t n) {
    printf("{ ");
    for (size_t i = 0; i < n; ++i) {
        if (B & (1ULL << i)) {
            printf("%zu ", i + 1);
        }
    }
    printf("}\n");
}

void generateHammingDistance1(size_t k, size_t n) {
    // Inizializza il subset base come primi k bit impostati a 1
    uint64_t B = (1ULL << k) - 1; // Es. per k=3: B = 0b111

    printf("Base subset:\n");
    printSubset(B, n);

    // Genera tutti i subset a semidistanza di Hamming unitaria
    for (size_t i = 0; i < k; ++i) {
        // Azzera il bit i-esimo del subset base
        uint64_t clear_mask = ~(1ULL << i);

        // Imposta il bit j (j >= k) nell'insieme differenza
        for (size_t j = k; j < n; ++j) {
            uint64_t set_mask = (1ULL << j);

            // Modifica il subset
            uint64_t modified = (B & clear_mask) | set_mask;

            // Stampa il nuovo subset
            printSubset(modified, n);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s k n\n", argv[0]);
        return 1;
    }

    size_t k = (size_t)atoi(argv[1]);
    size_t n = (size_t)atoi(argv[2]);

    if (k < 3 || n < 2 * k || n > MAX_N) {
        fprintf(stderr, "Errore: Assicurati che 2 < k e 2k <= n <= %d.\n", MAX_N);
        return 1;
    }

    generateHammingDistance1(k, n);
    return 0;
}
/** EOF: ksubset_DL.c **/