#include "batch_list.h"

#include "game/segment2.h"

static inline struct BatchArray* batch_array_alloc(struct AllocOnlyPool *pool, int count)
{
    struct BatchArray* batches = alloc_only_pool_alloc(pool, sizeof(struct BatchArray) + count * sizeof(struct Batch));
    batches->count = count;
    return batches;
}

static inline void batch_setup(struct BatchArray* arr, int idx, const void* start, const void* end)
{
    struct Batch* batch = &arr->batches[idx];
    batch->startDl = start;
    batch->endDl   = end;
}

struct BatchArray* batch_list_objects_alloc_xlu_decal(struct AllocOnlyPool *pool)
{
    struct BatchArray* arr = batch_array_alloc(pool, BATCH_TRANSPARENT_DECAL_COUNT);
    batch_setup(arr, BATCH_TRANSPARENT_DECAL_SHADOW_CIRCLE, dl_shadow_circle, dl_shadow_end);
    batch_setup(arr, BATCH_TRANSPARENT_DECAL_SHADOW_SQUARE, dl_shadow_square, dl_shadow_end);
    return arr;
}

struct BatchArray* batch_list_objects_alloc_cld(struct AllocOnlyPool *pool)
{
    struct BatchArray* arr = batch_array_alloc(pool, BATCH_CLD_COUNT);
    batch_setup(arr, BATCH_CLD_SHADOW_CIRCLE, dl_shadow_circle, dl_shadow_end);
    batch_setup(arr, BATCH_CLD_SHADOW_SQUARE, dl_shadow_square, dl_shadow_end);
    return arr;
}
