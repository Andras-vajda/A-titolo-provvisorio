#include "lotto.h"
#include "lotto_catalog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_CHECK(expression)                                      \
    do {                                                            \
        if (!(expression)) {                                        \
            fprintf(stderr, "FAILED: %s, line %lu: %s\n",          \
                    __FILE__, (unsigned long)__LINE__, #expression);\
            return LOTTO_FALSE;                                     \
        }                                                           \
    } while (0)

static int sets_equal(const lotto_set_t *left, const lotto_set_t *right)
{
    unsigned int i;

    if (left->size != right->size) {
        return LOTTO_FALSE;
    }
    for (i = 0U; i < left->size; ++i) {
        if (left->point[i] != right->point[i]) {
            return LOTTO_FALSE;
        }
    }
    return LOTTO_TRUE;
}

static int test_development_and_validation(void)
{
    static const lotto_block_t expected[] = {
        { 5U, { 0U, 1U, 2U, 4U, 8U } },
        { 5U, { 0U, 1U, 3U, 5U, 8U } },
        { 5U, { 0U, 2U, 3U, 4U, 8U } },
        { 5U, { 0U, 2U, 6U, 7U, 9U } },
        { 5U, { 1U, 2U, 3U, 5U, 8U } },
        { 5U, { 1U, 3U, 6U, 7U, 9U } },
        { 5U, { 4U, 5U, 6U, 7U, 9U } }
    };
    lotto_workspace_t workspace;
    lotto_block_t blocks[LOTTO_MAX_BLOCKS];
    lotto_validation_t validation;
    lotto_metrics_t metrics;
    lotto_status_t status;
    size_t block_count;
    size_t i;

    block_count = 0U;
    status = lotto_develop(
        &lotto_construction_10_5_4_3,
        blocks,
        LOTTO_MAX_BLOCKS,
        &block_count,
        &workspace);
    TEST_CHECK(status == LOTTO_STATUS_OK);
    TEST_CHECK(block_count == 7U);

    for (i = 0U; i < block_count; ++i) {
        TEST_CHECK(sets_equal(&blocks[i], &expected[i]) != LOTTO_FALSE);
    }

    status = lotto_validate(
        &lotto_construction_10_5_4_3.spec,
        blocks,
        block_count,
        &validation);
    TEST_CHECK(status == LOTTO_STATUS_OK);
    TEST_CHECK(validation.is_valid != LOTTO_FALSE);
    TEST_CHECK(validation.targets_checked == 210UL);
    TEST_CHECK(validation.minimum_multiplicity == 1U);
    TEST_CHECK(validation.maximum_multiplicity == 4U);
    TEST_CHECK(validation.minimum_best_intersection == 3U);

    status = lotto_measure(
        &lotto_construction_10_5_4_3.spec,
        blocks,
        block_count,
        &metrics);
    TEST_CHECK(status == LOTTO_STATUS_OK);
    TEST_CHECK(metrics.target_count == 210UL);
    TEST_CHECK(metrics.total_cover_multiplicity == 385UL);
    TEST_CHECK(metrics.cover_multiplicity_histogram[0] == 0UL);
    TEST_CHECK(metrics.cover_multiplicity_histogram[1] == 84UL);
    TEST_CHECK(metrics.cover_multiplicity_histogram[2] == 84UL);
    TEST_CHECK(metrics.cover_multiplicity_histogram[3] == 35UL);
    TEST_CHECK(metrics.cover_multiplicity_histogram[4] == 7UL);
    TEST_CHECK(metrics.pair_intersection_histogram[1] == 8UL);
    TEST_CHECK(metrics.pair_intersection_histogram[2] == 4UL);
    TEST_CHECK(metrics.pair_intersection_histogram[3] == 7UL);
    TEST_CHECK(metrics.pair_intersection_histogram[4] == 2UL);

    return LOTTO_TRUE;
}

static int test_each_block_is_essential(void)
{
    lotto_workspace_t workspace;
    lotto_block_t blocks[LOTTO_MAX_BLOCKS];
    lotto_block_t reduced[LOTTO_MAX_BLOCKS];
    lotto_validation_t validation;
    lotto_status_t status;
    size_t block_count;
    size_t removed;
    size_t source;
    size_t destination;

    block_count = 0U;
    status = lotto_develop(
        &lotto_construction_10_5_4_3,
        blocks,
        LOTTO_MAX_BLOCKS,
        &block_count,
        &workspace);
    TEST_CHECK(status == LOTTO_STATUS_OK);

    for (removed = 0U; removed < block_count; ++removed) {
        destination = 0U;
        for (source = 0U; source < block_count; ++source) {
            if (source != removed) {
                reduced[destination] = blocks[source];
                ++destination;
            }
        }

        status = lotto_validate(
            &lotto_construction_10_5_4_3.spec,
            reduced,
            destination,
            &validation);
        TEST_CHECK(status == LOTTO_STATUS_OK);
        TEST_CHECK(validation.is_valid == LOTTO_FALSE);
        TEST_CHECK(validation.counterexample.size == 4U);
    }

    return LOTTO_TRUE;
}

static int test_transactional_capacity_failure(void)
{
    lotto_workspace_t workspace;
    lotto_block_t blocks[7];
    lotto_status_t status;
    size_t block_count;

    memset(blocks, 0xA5, sizeof(blocks));
    block_count = 123U;

    status = lotto_develop(
        &lotto_construction_10_5_4_3,
        blocks,
        6U,
        &block_count,
        &workspace);

    TEST_CHECK(status == LOTTO_STATUS_INSUFFICIENT_CAPACITY);
    TEST_CHECK(block_count == 123U);
    TEST_CHECK(blocks[0].size != 5U);

    return LOTTO_TRUE;
}

static int test_metamorphic_relabelling(void)
{
    static const lotto_permutation_t relabelling = {
        { 9U, 8U, 7U, 6U, 5U, 4U, 3U, 2U, 1U, 0U,
          10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U }
    };
    lotto_workspace_t workspace;
    lotto_block_t blocks[LOTTO_MAX_BLOCKS];
    lotto_block_t relabelled[LOTTO_MAX_BLOCKS];
    lotto_validation_t validation;
    lotto_status_t status;
    size_t block_count;

    block_count = 0U;
    status = lotto_develop(
        &lotto_construction_10_5_4_3,
        blocks,
        LOTTO_MAX_BLOCKS,
        &block_count,
        &workspace);
    TEST_CHECK(status == LOTTO_STATUS_OK);

    status = lotto_relabel(
        &lotto_construction_10_5_4_3.spec,
        blocks,
        block_count,
        &relabelling,
        relabelled,
        LOTTO_MAX_BLOCKS);
    TEST_CHECK(status == LOTTO_STATUS_OK);

    status = lotto_validate(
        &lotto_construction_10_5_4_3.spec,
        relabelled,
        block_count,
        &validation);
    TEST_CHECK(status == LOTTO_STATUS_OK);
    TEST_CHECK(validation.is_valid != LOTTO_FALSE);
    TEST_CHECK(validation.targets_checked == 210UL);

    return LOTTO_TRUE;
}

int main(void)
{
    int passed;

    passed = LOTTO_TRUE;
    passed = passed && test_development_and_validation();
    passed = passed && test_each_block_is_essential();
    passed = passed && test_transactional_capacity_failure();
    passed = passed && test_metamorphic_relabelling();

    if (passed != LOTTO_FALSE) {
        puts("All lotto tests passed.");
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
