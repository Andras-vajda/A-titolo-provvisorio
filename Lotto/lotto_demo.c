#include "lotto.h"
#include "lotto_catalog.h"

#include <stdio.h>
#include <stdlib.h>

static void print_set(const lotto_set_t *set)
{
    unsigned int i;

    putchar('{');
    for (i = 0U; i < set->size; ++i) {
        if (i > 0U) {
            fputs(", ", stdout);
        }
        printf("%u", (unsigned int)set->point[i]);
    }
    putchar('}');
}

int main(void)
{
    lotto_workspace_t workspace;
    lotto_block_t blocks[LOTTO_MAX_BLOCKS];
    lotto_validation_t validation;
    lotto_metrics_t metrics;
    lotto_status_t status;
    size_t block_count;
    size_t i;
    unsigned int value;

    block_count = 0U;
    status = lotto_develop(
        &lotto_construction_10_5_4_3,
        blocks,
        LOTTO_MAX_BLOCKS,
        &block_count,
        &workspace);
    if (status != LOTTO_STATUS_OK) {
        fprintf(stderr, "Construction failed: %s\n",
                lotto_status_string(status));
        return EXIT_FAILURE;
    }

    status = lotto_validate(
        &lotto_construction_10_5_4_3.spec,
        blocks,
        block_count,
        &validation);
    if (status != LOTTO_STATUS_OK) {
        fprintf(stderr, "Validation failed: %s\n",
                lotto_status_string(status));
        return EXIT_FAILURE;
    }

    status = lotto_measure(
        &lotto_construction_10_5_4_3.spec,
        blocks,
        block_count,
        &metrics);
    if (status != LOTTO_STATUS_OK) {
        fprintf(stderr, "Measurement failed: %s\n",
                lotto_status_string(status));
        return EXIT_FAILURE;
    }

    printf("%s\n", lotto_construction_10_5_4_3.name);
    printf("Blocks: %lu\n", (unsigned long)block_count);
    for (i = 0U; i < block_count; ++i) {
        printf("B%lu = ", (unsigned long)i);
        print_set(&blocks[i]);
        putchar('\n');
    }

    printf("\nValid: %s\n", validation.is_valid ? "yes" : "no");
    printf("Targets checked: %lu\n", validation.targets_checked);
    printf("Minimum cover multiplicity: %u\n",
           validation.minimum_multiplicity);
    printf("Maximum cover multiplicity: %u\n",
           validation.maximum_multiplicity);
    printf("Minimum best intersection: %u\n",
           validation.minimum_best_intersection);

    printf("\nPoint replications:\n");
    for (value = 0U;
         value < lotto_construction_10_5_4_3.spec.n;
         ++value) {
        printf("r(%u) = %u\n", value, metrics.point_replication[value]);
    }

    printf("\nCover multiplicity histogram:\n");
    for (value = 0U; value <= (unsigned int)block_count; ++value) {
        if (metrics.cover_multiplicity_histogram[value] != 0UL) {
            printf("m = %u: %lu\n", value,
                   metrics.cover_multiplicity_histogram[value]);
        }
    }

    return validation.is_valid ? EXIT_SUCCESS : EXIT_FAILURE;
}
