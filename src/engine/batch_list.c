#include "batch_list.h"

#include "game/segment2.h"

static const struct BatchDisplayLists DecalBatchesTransparent[] = {
    [ BATCH_TRANSPARENT_DECAL_SHADOW_CIRCLE ] = { dl_shadow_circle, dl_shadow_end },
    [ BATCH_TRANSPARENT_DECAL_SHADOW_SQUARE ] = { dl_shadow_square, dl_shadow_end },
};
STATIC_ASSERT(BATCH_TRANSPARENT_DECAL_COUNT == sizeof(DecalBatchesTransparent) / sizeof(*DecalBatchesTransparent), "Mismatch in transparent decal batch count");

static const struct BatchDisplayLists DecalBatchesCLD[] = {
    [ BATCH_CLD_SHADOW_CIRCLE ] = { dl_shadow_circle, dl_shadow_end },
    [ BATCH_CLD_SHADOW_SQUARE ] = { dl_shadow_square, dl_shadow_end },
};
STATIC_ASSERT(BATCH_CLD_COUNT == sizeof(DecalBatchesCLD) / sizeof(*DecalBatchesCLD), "Mismatch in CLD batch count");

static inline struct BatchArray* batch_array_alloc(struct AllocOnlyPool *pool, int count, const struct BatchDisplayLists* dls) {
    struct BatchArray* batches = alloc_only_pool_alloc(pool, sizeof(struct BatchArray) + count * sizeof(struct DisplayListLinks));
    batches->count = count;
    batches->batchDLs = dls;
    return batches;
}

struct BatchArray* batch_list_objects_alloc_xlu_decal(struct AllocOnlyPool *pool) {
    return batch_array_alloc(pool, BATCH_TRANSPARENT_DECAL_COUNT, DecalBatchesTransparent);
}

struct BatchArray* batch_list_objects_alloc_cld(struct AllocOnlyPool *pool) {
    return batch_array_alloc(pool, BATCH_CLD_COUNT, DecalBatchesCLD);
}
