#pragma once

/******************
 * AUDIO SETTINGS *
 ******************/

/**
 * Do not restart the music on cap grabs.
 */
#define PERSISTENT_CAP_MUSIC

/**
 * Red coins should use the unchanging JP sound effect.
 */
// #define JP_RED_COIN_SOUND

/** 
 * In vanilla, Mario's sounds are pitch shifted in real time in order to maintain a sense of consistency. This can be annoying when replacing Mario's sounds.
 * This define will disable this behavior, making sound replacements easier to manage. Use of this is not recommended unless you're actually replacing Mario's sounds.
 * When toggling this define, you will need to run `make clean` for it to apply.
 */
// #define DISABLE_MARIO_PITCH_SHIFTING

/** 
 * Enables the funky sound processing that takes place with sound effects, that can sometimes cause sounds like the waterfall in castle grounds to sound more unpleasant than necessary.
 * This is not the same thing as enabling stereo audio, but it does completely represent the sole differences between the Stereo and Headset sound options.
 * Because of this, the Headset sound option will also not appear in the file select menu with this disabled.
 */
// #define ENABLE_HEADSET_STEREO_EFFECTS

/**
 * Increase audio heap size to allow for larger/more custom sequences/banks/sfx to be imported without causing issues (not supported for SH).
 * Note that this is REQUIRED in order to use the extended 0C audio bank. Do not disable this if you're planning on using it. 
 */
#define EXPAND_AUDIO_HEAP

/**
 * The maximum number of notes (sfx inclusive) that can sound at any given time (not supported for SH).
 * Lower values may cause notes to get cut more easily but can potentially improve performance slightly.
 * Lower values may cause problems with streamed audio if a sequence used for it is missing channel priority data.
 * Vanilla by default only generally allocates 16 or 20 notes at once. Memory usage is always determined by the largest of the two values here (~7200 bytes per US/JP note).
 */
#define MAX_SIMULTANEOUS_NOTES_EMULATOR 40
#define MAX_SIMULTANEOUS_NOTES_CONSOLE 24

/** 
 * Uses a much better implementation of reverb over vanilla's fake echo reverb. Great for caves or eerie levels, as well as just a better audio experience in general.
 * Reverb presets can be configured in audio/data.c to meet desired aesthetic/performance needs. More detailed usage info can also be found on the HackerSM64 Wiki page.
 */
// #define BETTER_REVERB
