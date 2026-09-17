#ifndef LOTTO_H_INCLUDED
#define LOTTO_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Conservative implementation limits.
 *
 * The mathematical representation is independent of machine word size:
 * points are stored explicitly and sets are kept in increasing order.
 */
#define LOTTO_MAX_POINTS       20U
#define LOTTO_MAX_SET_SIZE     10U
#define LOTTO_MAX_BLOCKS      128U
#define LOTTO_MAX_GENERATORS    8U
#define LOTTO_MAX_BASE_BLOCKS  16U

#define LOTTO_FALSE 0
#define LOTTO_TRUE  1

typedef unsigned char lotto_point_t;

typedef struct lotto_set_s {
    unsigned int size;
    lotto_point_t point[LOTTO_MAX_SET_SIZE];
} lotto_set_t;

typedef lotto_set_t lotto_block_t;

typedef struct lotto_spec_s {
    unsigned int n;
    unsigned int k;
    unsigned int p;
    unsigned int t;
} lotto_spec_t;

typedef struct lotto_permutation_s {
    lotto_point_t image[LOTTO_MAX_POINTS];
} lotto_permutation_t;

typedef struct lotto_construction_s {
    const char *name;
    lotto_spec_t spec;
    const lotto_block_t *base_blocks;
    size_t base_block_count;
    const lotto_permutation_t *generators;
    size_t generator_count;
    const unsigned int *expected_orbit_sizes;
    size_t expected_block_count;
} lotto_construction_t;

typedef enum lotto_status_e {
    LOTTO_STATUS_OK = 0,
    LOTTO_STATUS_NULL_ARGUMENT,
    LOTTO_STATUS_INVALID_SPECIFICATION,
    LOTTO_STATUS_INVALID_SET,
    LOTTO_STATUS_INVALID_PERMUTATION,
    LOTTO_STATUS_TOO_MANY_BASE_BLOCKS,
    LOTTO_STATUS_TOO_MANY_GENERATORS,
    LOTTO_STATUS_INSUFFICIENT_CAPACITY,
    LOTTO_STATUS_DUPLICATE_BASE_ORBIT,
    LOTTO_STATUS_UNEXPECTED_ORBIT_SIZE,
    LOTTO_STATUS_UNEXPECTED_BLOCK_COUNT,
    LOTTO_STATUS_ARITHMETIC_OVERFLOW,
    LOTTO_STATUS_INTERNAL_INVARIANT
} lotto_status_t;

typedef struct lotto_workspace_s {
    lotto_block_t block[LOTTO_MAX_BLOCKS];
    size_t queue[LOTTO_MAX_BLOCKS];
} lotto_workspace_t;

typedef struct lotto_validation_s {
    int is_valid;
    unsigned long targets_checked;
    unsigned int minimum_multiplicity;
    unsigned int maximum_multiplicity;
    unsigned int minimum_best_intersection;
    lotto_set_t counterexample;
} lotto_validation_t;

typedef struct lotto_metrics_s {
    unsigned long target_count;
    unsigned long total_cover_multiplicity;
    unsigned int minimum_cover_multiplicity;
    unsigned int maximum_cover_multiplicity;
    unsigned int point_replication[LOTTO_MAX_POINTS];
    unsigned long pair_intersection_histogram[LOTTO_MAX_SET_SIZE + 1U];
    unsigned long cover_multiplicity_histogram[LOTTO_MAX_BLOCKS + 1U];
} lotto_metrics_t;

/*
 * Caller preconditions: input objects remain stable during the call.
 * Workspace, output array and output count occupy disjoint storage and
 * do not overlap any construction data read by the operation. The output
 * array has at least output_capacity elements. These are caller obligations,
 * not properties established by null checks or by const-qualified pointers.
 * On any reported error, output_blocks and *output_count are unchanged;
 * the workspace is scratch storage and may have been modified.
 */
lotto_status_t lotto_develop(
    const lotto_construction_t *construction,
    lotto_block_t *output_blocks,
    size_t output_capacity,
    size_t *output_count,
    lotto_workspace_t *workspace);

/*
 * On OK, is_valid is definitive. An invalid design stops enumeration at
 * the first uncovered target (included in targets_checked). Its extrema
 * describe only that visited prefix: minimum_multiplicity is globally zero,
 * but maximum_multiplicity and minimum_best_intersection need not be global.
 * For a valid design enumeration is complete. On a reported error the
 * validation object is unchanged. Output must not overlap input storage.
 */
lotto_status_t lotto_validate(
    const lotto_spec_t *spec,
    const lotto_block_t *blocks,
    size_t block_count,
    lotto_validation_t *validation);

/*
 * Enumerates every target, including for invalid designs. Returns complete
 * coverage metrics, but does not compute minimum_best_intersection.
 * On a reported error metrics is unchanged. Output must not overlap inputs.
 */
lotto_status_t lotto_measure(
    const lotto_spec_t *spec,
    const lotto_block_t *blocks,
    size_t block_count,
    lotto_metrics_t *metrics);

/*
 * Input objects remain stable during the call. The output array must be
 * disjoint from the specification and permutation; input and output block
 * arrays must be disjoint or exactly identical (in-place relabelling).
 * Partially overlapping arrays are not supported.
 */
lotto_status_t lotto_relabel(
    const lotto_spec_t *spec,
    const lotto_block_t *input_blocks,
    size_t block_count,
    const lotto_permutation_t *permutation,
    lotto_block_t *output_blocks,
    size_t output_capacity);

const char *lotto_status_string(lotto_status_t status);

#ifdef __cplusplus
}
#endif

#endif
