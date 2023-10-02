#include <ultra64.h>
#include "global_object_fields.h"
#include "object_helpers.h"

// music_touch.inc.c

void bhv_play_music_track_when_touched_loop(void) {
    if (o->oAction == 0 && o->oDistanceToMario < 200.0f) {
        play_puzzle_jingle();
        o->oAction++;
    }
}