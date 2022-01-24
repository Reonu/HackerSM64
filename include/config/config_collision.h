#pragma once

/**********************
 * COLLISION SETTINGS *
 **********************/

// Reduces some find_floor calls, at the cost of some barely noticeable smoothness in Mario's visual movement in a few actions at higher speeds.
// The defined number is the forward speed threshold before the change is active, since it's only noticeable at lower speeds.
#define FAST_FLOOR_ALIGN 10

// Automatically calculates the optimal collision distance for an object based on its vertices.
#define AUTO_COLLISION_DISTANCE

// Allows all surfaces types to have force, (doesn't require setting force, just allows it to be optional).
#define ALL_SURFACES_HAVE_FORCE

// Number of walls that can push Mario at once. Vanilla is 4.
#define MAX_REFERENCED_WALLS 4

// Collision data is the type that the collision system uses. All data by default is stored as an s16, but you may change it to s32.
// Naturally, that would double the size of all collision data, but would allow you to use 32 bit values instead of 16.
// Rooms are s8 in vanilla, but if you somehow have more than 255 rooms, you may raise this number.
// Currently, they *must* say as s8, because the room tables generated by literally anything are explicitly u8 and don't use a macro, making this currently infeasable.
#define COLLISION_DATA_TYPE s16
#define ROOM_DATA_TYPE s8

// Makes find_floor and find_ceil calls return the highest/lowest surface instead of just the first valid surface.
#define SLOPE_FIX

// Checks for ceilings from Mario's actual height instead of from the floor height.
#define EXPOSED_CEILINGS_FIX

// Uses the correct HOLP height rather than Mario's height when dropping a held object.
#define HOLP_HEIGHT_FIX

// Mario's normal hitbox height.
#define MARIO_HITBOX_HEIGHT 160

// Mario's hitbox height when in certain actions such as crouching or crawling.
#define MARIO_SHORT_HITBOX_HEIGHT 100

// The radius of Mario's collision when checking for walls.
#define MARIO_COLLISION_RADIUS 50

// The minimum number of units above Mario's origin a floor must be for Mario to be able to ledge grab on it.
#define LEDGE_GRAB_MIN_HEIGHT 100

// The maximum number of units above Mario's origin a floor must be for Mario to be able to ledge grab on it.
#define LEDGE_GRAB_MAX_HEIGHT 160

// Allow Mario to be in OOB areas.
// On console, this crashes soon after Mario reaches about 65536 units from the area's origin.
// #define ALLOW_OOB
