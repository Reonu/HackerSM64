#pragma once

#include <ultra64.h>

#include "types.h"

#include "crash_screen/cs_draw.h"


#define NUM_SHOWN_SCROLL_THREADS        9
#define NUM_SHOWN_TOTAL_THREADS         (NUM_SHOWN_SCROLL_THREADS + 1)

#define CS_POPUP_THREADS_NUM_CHARS_X    30
#define CS_POPUP_THREADS_WIDTH          TEXT_WIDTH(CS_POPUP_THREADS_NUM_CHARS_X) // 180
#define CS_POPUP_THREADS_X1             (SCREEN_CENTER_X - (CS_POPUP_THREADS_WIDTH / 2)) // 100
#define CS_POPUP_THREADS_X2             (SCREEN_CENTER_X + (CS_POPUP_THREADS_WIDTH / 2)) // 220
#define CS_POPUP_THREADS_BG_X1          (CS_POPUP_THREADS_X1 - 3)
#define CS_POPUP_THREADS_BG_X2          (CS_POPUP_THREADS_X2 + 2)
#define CS_POPUP_THREADS_TEXT_X1        ((CRASH_SCREEN_NUM_CHARS_X / 2) - (CS_POPUP_THREADS_NUM_CHARS_X / 2))
#define CS_POPUP_THREADS_TEXT_X2        (CS_POPUP_THREADS_TEXT_X1 + CS_POPUP_THREADS_NUM_CHARS_X)
#define CS_POPUP_THREADS_BG_WIDTH       (CS_POPUP_THREADS_BG_X2 - CS_POPUP_THREADS_BG_X1)

#define CS_POPUP_THREADS_NUM_CHARS_Y    (NUM_SHOWN_TOTAL_THREADS * 2)
#define CS_POPUP_THREADS_HEIGHT         TEXT_HEIGHT(CS_POPUP_THREADS_NUM_CHARS_Y) // 200
#define CS_POPUP_THREADS_STARTLINE      2
#define CS_POPUP_THREADS_Y1             (TEXT_Y(CS_POPUP_THREADS_STARTLINE) - (TEXT_HEIGHT(1) / 2))
#define CS_POPUP_THREADS_Y2             (CS_POPUP_THREADS_Y1 + CS_POPUP_THREADS_HEIGHT)
#define CS_POPUP_THREADS_BG_Y1          (CS_POPUP_THREADS_Y1 - 2)
#define CS_POPUP_THREADS_BG_Y2          (CS_POPUP_THREADS_Y2 - 1)
#define CS_POPUP_THREADS_BG_HEIGHT      (CS_POPUP_THREADS_BG_Y2 - CS_POPUP_THREADS_BG_Y1)


extern struct CSPopup gCSPopup_threads;


void cs_open_threads(void);

void cs_print_thread_info(CSTextCoord_u32 charStartX, CSScreenCoord_u32 y, OSThread* thread, _Bool showAddress, _Bool showViewing);

ALWAYS_INLINE void cs_draw_row_box_thread(CSScreenCoord_u32 x, CSScreenCoord_u32 y, RGBA32 color) {
    cs_draw_row_box_w((x + 2), (CS_POPUP_THREADS_BG_WIDTH - 3), y, color);
}
