#include "lotto.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

static lotto_status_t lotto_check_spec(const lotto_spec_t *spec)
{
    if (spec == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }

    if (spec->n == 0U || spec->n > LOTTO_MAX_POINTS ||
        spec->k == 0U || spec->k > LOTTO_MAX_SET_SIZE ||
        spec->p == 0U || spec->p > LOTTO_MAX_SET_SIZE ||
        spec->t == 0U || spec->k > spec->n || spec->p > spec->n ||
        spec->t > spec->k || spec->t > spec->p) {
        return LOTTO_STATUS_INVALID_SPECIFICATION;
    }

    return LOTTO_STATUS_OK;
}

static lotto_status_t lotto_check_set(
    const lotto_set_t *set,
    unsigned int expected_size,
    unsigned int n)
{
    unsigned int i;

    if (set == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }

    if (set->size != expected_size || set->size > LOTTO_MAX_SET_SIZE) {
        return LOTTO_STATUS_INVALID_SET;
    }

    for (i = 0U; i < set->size; ++i) {
        if ((unsigned int)set->point[i] >= n) {
            return LOTTO_STATUS_INVALID_SET;
        }
        if (i > 0U && set->point[i - 1U] >= set->point[i]) {
            return LOTTO_STATUS_INVALID_SET;
        }
    }

    return LOTTO_STATUS_OK;
}

static lotto_status_t lotto_check_permutation(
    const lotto_permutation_t *permutation,
    unsigned int n)
{
    int seen[LOTTO_MAX_POINTS];
    unsigned int i;
    unsigned int image;

    if (permutation == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }

    for (i = 0U; i < LOTTO_MAX_POINTS; ++i) {
        seen[i] = LOTTO_FALSE;
    }

    for (i = 0U; i < n; ++i) {
        image = (unsigned int)permutation->image[i];
        if (image >= n || seen[image] != LOTTO_FALSE) {
            return LOTTO_STATUS_INVALID_PERMUTATION;
        }
        seen[image] = LOTTO_TRUE;
    }

    return LOTTO_STATUS_OK;
}

static int lotto_set_compare(const lotto_set_t *left, const lotto_set_t *right)
{
    unsigned int i;
    unsigned int common_size;

    common_size = left->size < right->size ? left->size : right->size;

    for (i = 0U; i < common_size; ++i) {
        if (left->point[i] < right->point[i]) {
            return -1;
        }
        if (left->point[i] > right->point[i]) {
            return 1;
        }
    }

    if (left->size < right->size) {
        return -1;
    }
    if (left->size > right->size) {
        return 1;
    }
    return 0;
}

static void lotto_sort_points(lotto_set_t *set)
{
    unsigned int i;
    unsigned int j;
    lotto_point_t value;

    for (i = 1U; i < set->size; ++i) {
        value = set->point[i];
        j = i;
        while (j > 0U && set->point[j - 1U] > value) {
            set->point[j] = set->point[j - 1U];
            --j;
        }
        set->point[j] = value;
    }
}

static void lotto_sort_blocks(lotto_block_t *blocks, size_t block_count)
{
    size_t i;
    size_t j;
    lotto_block_t value;

    for (i = 1U; i < block_count; ++i) {
        value = blocks[i];
        j = i;
        while (j > 0U && lotto_set_compare(&blocks[j - 1U], &value) > 0) {
            blocks[j] = blocks[j - 1U];
            --j;
        }
        blocks[j] = value;
    }
}

static int lotto_find_block(
    const lotto_block_t *blocks,
    size_t block_count,
    const lotto_block_t *block,
    size_t *index)
{
    size_t i;

    for (i = 0U; i < block_count; ++i) {
        if (lotto_set_compare(&blocks[i], block) == 0) {
            if (index != NULL) {
                *index = i;
            }
            return LOTTO_TRUE;
        }
    }

    return LOTTO_FALSE;
}

static void lotto_apply_permutation_unchecked(
    const lotto_block_t *input,
    const lotto_permutation_t *permutation,
    lotto_block_t *output)
{
    unsigned int i;

    output->size = input->size;
    for (i = 0U; i < input->size; ++i) {
        output->point[i] = permutation->image[input->point[i]];
    }
    lotto_sort_points(output);
}

static unsigned int lotto_intersection_size(
    const lotto_set_t *left,
    const lotto_set_t *right)
{
    unsigned int i;
    unsigned int j;
    unsigned int count;

    i = 0U;
    j = 0U;
    count = 0U;

    while (i < left->size && j < right->size) {
        if (left->point[i] < right->point[j]) {
            ++i;
        } else if (left->point[i] > right->point[j]) {
            ++j;
        } else {
            ++count;
            ++i;
            ++j;
        }
    }

    return count;
}

static void lotto_combination_first(
    lotto_set_t *combination,
    unsigned int p)
{
    unsigned int i;

    combination->size = p;
    for (i = 0U; i < p; ++i) {
        combination->point[i] = (lotto_point_t)i;
    }
}

static int lotto_combination_next(
    lotto_set_t *combination,
    unsigned int n)
{
    unsigned int i;
    unsigned int j;
    unsigned int limit;

    i = combination->size;
    while (i > 0U) {
        --i;
        limit = n - combination->size + i;
        if ((unsigned int)combination->point[i] < limit) {
            combination->point[i] =
                (lotto_point_t)((unsigned int)combination->point[i] + 1U);
            for (j = i + 1U; j < combination->size; ++j) {
                combination->point[j] =
                    (lotto_point_t)((unsigned int)combination->point[j - 1U] + 1U);
            }
            return LOTTO_TRUE;
        }
    }

    return LOTTO_FALSE;
}

static lotto_status_t lotto_safe_add_ulong(
    unsigned long left,
    unsigned long right,
    unsigned long *result)
{
    if (result == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }
    if (ULONG_MAX - left < right) {
        return LOTTO_STATUS_ARITHMETIC_OVERFLOW;
    }
    *result = left + right;
    return LOTTO_STATUS_OK;
}

static lotto_status_t lotto_check_design(
    const lotto_spec_t *spec,
    const lotto_block_t *blocks,
    size_t block_count)
{
    lotto_status_t status;
    size_t i;
    size_t j;

    status = lotto_check_spec(spec);
    if (status != LOTTO_STATUS_OK) {
        return status;
    }
    if (blocks == NULL || block_count == 0U || block_count > LOTTO_MAX_BLOCKS) {
        return blocks == NULL ? LOTTO_STATUS_NULL_ARGUMENT :
            LOTTO_STATUS_INVALID_SET;
    }

    for (i = 0U; i < block_count; ++i) {
        status = lotto_check_set(&blocks[i], spec->k, spec->n);
        if (status != LOTTO_STATUS_OK) {
            return status;
        }
        for (j = 0U; j < i; ++j) {
            if (lotto_set_compare(&blocks[i], &blocks[j]) == 0) {
                return LOTTO_STATUS_INVALID_SET;
            }
        }
    }

    return LOTTO_STATUS_OK;
}

lotto_status_t lotto_develop(
    const lotto_construction_t *construction,
    lotto_block_t *output_blocks,
    size_t output_capacity,
    size_t *output_count,
    lotto_workspace_t *workspace)
{
    lotto_status_t status;
    size_t base_index;
    size_t generator_index;
    size_t orbit_start;
    size_t queue_head;
    size_t queue_tail;
    size_t found_index;
    size_t total_count;
    size_t orbit_size;
    lotto_block_t image;

    if (construction == NULL || output_blocks == NULL ||
        output_count == NULL || workspace == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }

    status = lotto_check_spec(&construction->spec);
    if (status != LOTTO_STATUS_OK) {
        return status;
    }
    if (construction->base_blocks == NULL ||
        construction->expected_orbit_sizes == NULL ||
        construction->base_block_count == 0U) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }
    if (construction->base_block_count > LOTTO_MAX_BASE_BLOCKS) {
        return LOTTO_STATUS_TOO_MANY_BASE_BLOCKS;
    }
    if (construction->generator_count > LOTTO_MAX_GENERATORS) {
        return LOTTO_STATUS_TOO_MANY_GENERATORS;
    }
    if (construction->generator_count > 0U && construction->generators == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }
    if (construction->expected_block_count > LOTTO_MAX_BLOCKS ||
        output_capacity < construction->expected_block_count) {
        return LOTTO_STATUS_INSUFFICIENT_CAPACITY;
    }

    for (base_index = 0U;
         base_index < construction->base_block_count;
         ++base_index) {
        status = lotto_check_set(
            &construction->base_blocks[base_index],
            construction->spec.k,
            construction->spec.n);
        if (status != LOTTO_STATUS_OK) {
            return status;
        }
    }

    for (generator_index = 0U;
         generator_index < construction->generator_count;
         ++generator_index) {
        status = lotto_check_permutation(
            &construction->generators[generator_index],
            construction->spec.n);
        if (status != LOTTO_STATUS_OK) {
            return status;
        }
    }

    total_count = 0U;

    for (base_index = 0U;
         base_index < construction->base_block_count;
         ++base_index) {
        orbit_start = total_count;

        if (lotto_find_block(
                workspace->block,
                total_count,
                &construction->base_blocks[base_index],
                &found_index) != LOTTO_FALSE) {
            return LOTTO_STATUS_DUPLICATE_BASE_ORBIT;
        }

        if (total_count >= LOTTO_MAX_BLOCKS) {
            return LOTTO_STATUS_INSUFFICIENT_CAPACITY;
        }

        workspace->block[total_count] =
            construction->base_blocks[base_index];
        workspace->queue[0] = total_count;
        ++total_count;
        queue_head = 0U;
        queue_tail = 1U;

        while (queue_head < queue_tail) {
            assert(workspace->queue[queue_head] < total_count);

            for (generator_index = 0U;
                 generator_index < construction->generator_count;
                 ++generator_index) {
                lotto_apply_permutation_unchecked(
                    &workspace->block[workspace->queue[queue_head]],
                    &construction->generators[generator_index],
                    &image);

                if (lotto_find_block(
                        workspace->block,
                        total_count,
                        &image,
                        &found_index) != LOTTO_FALSE) {
                    if (found_index < orbit_start) {
                        return LOTTO_STATUS_DUPLICATE_BASE_ORBIT;
                    }
                } else {
                    if (total_count >= LOTTO_MAX_BLOCKS ||
                        queue_tail >= LOTTO_MAX_BLOCKS) {
                        return LOTTO_STATUS_INSUFFICIENT_CAPACITY;
                    }
                    workspace->block[total_count] = image;
                    workspace->queue[queue_tail] = total_count;
                    ++total_count;
                    ++queue_tail;
                }
            }
            ++queue_head;
        }

        orbit_size = total_count - orbit_start;
        if (orbit_size !=
            (size_t)construction->expected_orbit_sizes[base_index]) {
            return LOTTO_STATUS_UNEXPECTED_ORBIT_SIZE;
        }
    }

    if (total_count != construction->expected_block_count) {
        return LOTTO_STATUS_UNEXPECTED_BLOCK_COUNT;
    }

    lotto_sort_blocks(workspace->block, total_count);
    memcpy(output_blocks, workspace->block,
           total_count * sizeof(output_blocks[0]));
    *output_count = total_count;

    return LOTTO_STATUS_OK;
}

lotto_status_t lotto_validate(
    const lotto_spec_t *spec,
    const lotto_block_t *blocks,
    size_t block_count,
    lotto_validation_t *validation)
{
    lotto_status_t status;
    lotto_validation_t result;
    lotto_set_t target;
    size_t block_index;
    unsigned int intersection;
    unsigned int best_intersection;
    unsigned int multiplicity;

    if (validation == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }

    status = lotto_check_design(spec, blocks, block_count);
    if (status != LOTTO_STATUS_OK) {
        return status;
    }

    memset(&result, 0, sizeof(result));
    result.is_valid = LOTTO_TRUE;
    result.minimum_multiplicity = UINT_MAX;
    result.minimum_best_intersection = UINT_MAX;

    lotto_combination_first(&target, spec->p);

    do {
        best_intersection = 0U;
        multiplicity = 0U;

        for (block_index = 0U; block_index < block_count; ++block_index) {
            intersection = lotto_intersection_size(&target, &blocks[block_index]);
            if (intersection > best_intersection) {
                best_intersection = intersection;
            }
            if (intersection >= spec->t) {
                ++multiplicity;
            }
        }

        if (multiplicity < result.minimum_multiplicity) {
            result.minimum_multiplicity = multiplicity;
        }
        if (multiplicity > result.maximum_multiplicity) {
            result.maximum_multiplicity = multiplicity;
        }
        if (best_intersection < result.minimum_best_intersection) {
            result.minimum_best_intersection = best_intersection;
        }

        if (result.targets_checked == ULONG_MAX) {
            return LOTTO_STATUS_ARITHMETIC_OVERFLOW;
        }
        ++result.targets_checked;

        if (multiplicity == 0U) {
            result.is_valid = LOTTO_FALSE;
            result.counterexample = target;
            *validation = result;
            return LOTTO_STATUS_OK;
        }
    } while (lotto_combination_next(&target, spec->n) != LOTTO_FALSE);

    result.counterexample.size = 0U;
    *validation = result;
    return LOTTO_STATUS_OK;
}

lotto_status_t lotto_measure(
    const lotto_spec_t *spec,
    const lotto_block_t *blocks,
    size_t block_count,
    lotto_metrics_t *metrics)
{
    lotto_status_t status;
    lotto_metrics_t result;
    lotto_set_t target;
    size_t i;
    size_t j;
    unsigned int point_index;
    unsigned int intersection;
    unsigned int multiplicity;
    unsigned long new_total;

    if (metrics == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }

    status = lotto_check_design(spec, blocks, block_count);
    if (status != LOTTO_STATUS_OK) {
        return status;
    }

    memset(&result, 0, sizeof(result));
    result.minimum_cover_multiplicity = UINT_MAX;

    for (i = 0U; i < block_count; ++i) {
        for (point_index = 0U; point_index < blocks[i].size; ++point_index) {
            ++result.point_replication[blocks[i].point[point_index]];
        }
    }

    for (i = 0U; i < block_count; ++i) {
        for (j = i + 1U; j < block_count; ++j) {
            intersection = lotto_intersection_size(&blocks[i], &blocks[j]);
            assert(intersection <= LOTTO_MAX_SET_SIZE);
            ++result.pair_intersection_histogram[intersection];
        }
    }

    lotto_combination_first(&target, spec->p);
    do {
        multiplicity = 0U;
        for (i = 0U; i < block_count; ++i) {
            if (lotto_intersection_size(&target, &blocks[i]) >= spec->t) {
                ++multiplicity;
            }
        }

        if (multiplicity > LOTTO_MAX_BLOCKS) {
            return LOTTO_STATUS_INTERNAL_INVARIANT;
        }
        ++result.cover_multiplicity_histogram[multiplicity];

        if (multiplicity < result.minimum_cover_multiplicity) {
            result.minimum_cover_multiplicity = multiplicity;
        }
        if (multiplicity > result.maximum_cover_multiplicity) {
            result.maximum_cover_multiplicity = multiplicity;
        }

        status = lotto_safe_add_ulong(
            result.total_cover_multiplicity,
            (unsigned long)multiplicity,
            &new_total);
        if (status != LOTTO_STATUS_OK) {
            return status;
        }
        result.total_cover_multiplicity = new_total;

        if (result.target_count == ULONG_MAX) {
            return LOTTO_STATUS_ARITHMETIC_OVERFLOW;
        }
        ++result.target_count;
    } while (lotto_combination_next(&target, spec->n) != LOTTO_FALSE);

    *metrics = result;
    return LOTTO_STATUS_OK;
}

lotto_status_t lotto_relabel(
    const lotto_spec_t *spec,
    const lotto_block_t *input_blocks,
    size_t block_count,
    const lotto_permutation_t *permutation,
    lotto_block_t *output_blocks,
    size_t output_capacity)
{
    lotto_status_t status;
    size_t i;
    size_t j;

    if (permutation == NULL || output_blocks == NULL) {
        return LOTTO_STATUS_NULL_ARGUMENT;
    }
    if (output_capacity < block_count) {
        return LOTTO_STATUS_INSUFFICIENT_CAPACITY;
    }

    status = lotto_check_design(spec, input_blocks, block_count);
    if (status != LOTTO_STATUS_OK) {
        return status;
    }
    status = lotto_check_permutation(permutation, spec->n);
    if (status != LOTTO_STATUS_OK) {
        return status;
    }

    for (i = 0U; i < block_count; ++i) {
        lotto_apply_permutation_unchecked(
            &input_blocks[i], permutation, &output_blocks[i]);
    }
    lotto_sort_blocks(output_blocks, block_count);

    for (i = 1U; i < block_count; ++i) {
        for (j = 0U; j < i; ++j) {
            if (lotto_set_compare(&output_blocks[i], &output_blocks[j]) == 0) {
                return LOTTO_STATUS_INTERNAL_INVARIANT;
            }
        }
    }

    return LOTTO_STATUS_OK;
}

const char *lotto_status_string(lotto_status_t status)
{
    switch (status) {
    case LOTTO_STATUS_OK:
        return "success";
    case LOTTO_STATUS_NULL_ARGUMENT:
        return "null argument";
    case LOTTO_STATUS_INVALID_SPECIFICATION:
        return "invalid mathematical specification";
    case LOTTO_STATUS_INVALID_SET:
        return "invalid or duplicate set";
    case LOTTO_STATUS_INVALID_PERMUTATION:
        return "invalid permutation";
    case LOTTO_STATUS_TOO_MANY_BASE_BLOCKS:
        return "too many base blocks";
    case LOTTO_STATUS_TOO_MANY_GENERATORS:
        return "too many generators";
    case LOTTO_STATUS_INSUFFICIENT_CAPACITY:
        return "insufficient capacity";
    case LOTTO_STATUS_DUPLICATE_BASE_ORBIT:
        return "base blocks belong to overlapping orbits";
    case LOTTO_STATUS_UNEXPECTED_ORBIT_SIZE:
        return "unexpected orbit size";
    case LOTTO_STATUS_UNEXPECTED_BLOCK_COUNT:
        return "unexpected block count";
    case LOTTO_STATUS_ARITHMETIC_OVERFLOW:
        return "arithmetic overflow";
    case LOTTO_STATUS_INTERNAL_INVARIANT:
        return "internal invariant violation";
    default:
        return "unknown status";
    }
}
