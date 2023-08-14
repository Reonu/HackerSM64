#include <ultra64.h>

#include <string.h>

#include "types.h"
#include "sm64.h"

#include "crash_screen/address_select.h"
#include "crash_screen/crash_controls.h"
#include "crash_screen/crash_draw.h"
#include "crash_screen/crash_main.h"
#include "crash_screen/crash_print.h"
#include "crash_screen/crash_settings.h"
#include "crash_screen/insn_disasm.h"
#include "crash_screen/map_parser.h"
#include "crash_screen/memory_read.h"

#include "page_disasm.h"


const enum ControlTypes disasmContList[] = {
    CONT_DESC_SWITCH_PAGE,
    CONT_DESC_SHOW_CONTROLS,
    CONT_DESC_CYCLE_DRAW,
    CONT_DESC_CURSOR_VERTICAL,
    CONT_DESC_JUMP_TO_ADDRESS,
#ifdef INCLUDE_DEBUG_MAP
    CONT_DESC_TOGGLE_FUNCTIONS,
#endif
    CONT_DESC_LIST_END,
};


static u32 sDisasmViewportIndex = 0x00000000;
static u32 sDisasmBranchStartX = 0; // The X position where branch arrows start.
static u32 sDisasmNumShownRows = 20;

#ifdef INCLUDE_DEBUG_MAP
static const RGBA32 sBranchColors[] = {
    COLOR_RGBA32_ORANGE,
    COLOR_RGBA32_LIME,
    COLOR_RGBA32_CYAN,
    COLOR_RGBA32_MAGENTA,
    COLOR_RGBA32_YELLOW,
    COLOR_RGBA32_PINK,
    COLOR_RGBA32_LIGHT_GRAY,
    COLOR_RGBA32_LIGHT_BLUE,
};

_Bool gFillBranchBuffer = FALSE;
static _Bool sContinueFillBranchBuffer = FALSE;

ALIGNED16 static struct BranchArrow sBranchArrows[DISASM_BRANCH_BUFFER_SIZE];
static u32 sNumBranchArrows = 0;

static Address sBranchBufferCurrAddr = 0x00000000;


void reset_branch_buffer(Address funcAddr) {
    bzero(sBranchArrows, sizeof(sBranchArrows));
    sNumBranchArrows = 0;

    sBranchBufferCurrAddr = funcAddr;
}
#endif

void disasm_init(void) {
    sDisasmViewportIndex = gSelectedAddress;

#ifdef INCLUDE_DEBUG_MAP
    gFillBranchBuffer            = FALSE;
    sContinueFillBranchBuffer    = FALSE;
    reset_branch_buffer((Address)NULL);
#endif
}

#ifdef INCLUDE_DEBUG_MAP
//! TODO: Optimize this as much as possible
//! TODO: Version that works without INCLUDE_DEBUG_MAP (check for branches relative to viewport, or selected insn only?)
//! TODO: gCSSettings[CS_OPT_DISASM_ARROW_MODE].val
// @returns whether to continue next frame.
_Bool disasm_fill_branch_buffer(const char* fname, Address funcAddr) {
    if (fname == NULL) {
        return FALSE;
    }

    s16 curBranchColorIndex;
    s32 curBranchX;

    if (sNumBranchArrows == 0) {
        // Start:
        curBranchColorIndex = 0;
        curBranchX = sDisasmBranchStartX;
    } else { //! TODO: Verify that this ordering is correct:
        // Continue:
        curBranchColorIndex = sBranchArrows[sNumBranchArrows - 1].colorIndex;
        curBranchX          = sBranchArrows[sNumBranchArrows - 1].xPos;
    }

    // Pick up where we left off.
    struct BranchArrow* currArrow = &sBranchArrows[sNumBranchArrows];

    OSTime startTime = osGetTime();
    while (TRUE) {
        // Too many entries searched.
        if (sBranchBufferCurrAddr > (funcAddr + DISASM_FUNCTION_SEARCH_MAX_OFFSET)) {
            return FALSE;
        }

        // Too many arrows for buffer.
        if (sNumBranchArrows >= DISASM_BRANCH_BUFFER_SIZE) {
            return FALSE;
        }

        // Check if we have left the function.
        const struct MapSymbol* symbol = get_map_symbol(sBranchBufferCurrAddr, SYMBOL_SEARCH_FORWARD);
        if (symbol != NULL) {
            if (!is_in_code_segment(symbol->addr)) {
                return FALSE;
            }
            if (funcAddr != symbol->addr) {
                return FALSE;
            }
        }

        // Get the offset for the current function;
        InsnData insn = { .raw = *(Word*)sBranchBufferCurrAddr }; //! TODO: Is this an unsafe read?
        s16 branchOffset = check_for_branch_offset(insn);

        if (branchOffset != 0x0000) { //! TODO: Verify ordering:
            curBranchX += DISASM_BRANCH_ARROW_SPACING;
            curBranchColorIndex = ((curBranchColorIndex + 1) % ARRAY_COUNT(sBranchColors));

            // Wrap around if extended past end of screen.
            if ((sDisasmBranchStartX + curBranchX) > CRASH_SCREEN_TEXT_X2) {
                curBranchX = DISASM_BRANCH_ARROW_OFFSET;
            }

            currArrow->startAddr    = sBranchBufferCurrAddr;
            currArrow->branchOffset = branchOffset;
            currArrow->colorIndex   = curBranchColorIndex;
            currArrow->xPos         = curBranchX;

            currArrow++;
            sNumBranchArrows++;
        }

        sBranchBufferCurrAddr += DISASM_STEP;

        // Searching took to long, so continue from the same place on the next frame.
        if ((osGetTime() - startTime) > FRAMES_TO_CYCLES(1)) { //! TODO: better version of this if possible
            return TRUE;
        }
    }

    return FALSE;
}
#endif

void draw_branch_arrow(s32 startLine, s32 endLine, s32 dist, RGBA32 color, u32 printLine) {
    s32 numShownRows = sDisasmNumShownRows;

    // Check to see if arrow is fully away from the screen.
    if (
        ((startLine >= 0           ) || (endLine >= 0           )) &&
        ((startLine <  numShownRows) || (endLine <  numShownRows))
    ) {
        s32 arrowStartHeight = (TEXT_Y(printLine + startLine) + 3);
        s32 arrowEndHeight   = (TEXT_Y(printLine +   endLine) + 3);

        if (startLine < 0) {
            arrowStartHeight = (TEXT_Y(printLine) - 1);
        } else if (startLine >= numShownRows) {
            arrowStartHeight = (TEXT_Y(printLine + numShownRows) - 2);
        } else {
            crash_screen_draw_rect((sDisasmBranchStartX + 1), arrowStartHeight, dist, 1, color);
        }

        if (endLine < 0) {
            arrowEndHeight = (TEXT_Y(printLine) - 1);
        } else if (endLine >= numShownRows) {
            arrowEndHeight = (TEXT_Y(printLine + numShownRows) - 2);
        } else {
            //! TODO: crash_screen_draw_triangle.
            u32 x = ((sDisasmBranchStartX + dist) - DISASM_BRANCH_ARROW_OFFSET);
            crash_screen_draw_rect((x + 0), (arrowEndHeight - 0), (DISASM_BRANCH_ARROW_OFFSET + 1), 1, color);
            // Arrow head.
            crash_screen_draw_rect((x + 1), (arrowEndHeight - 1), 1, 3, color);
            crash_screen_draw_rect((x + 2), (arrowEndHeight - 2), 1, 5, color);
        }

        s32 height = abss(arrowEndHeight - arrowStartHeight);

        // Middle of arrow.
        crash_screen_draw_rect((sDisasmBranchStartX + dist), MIN(arrowStartHeight, arrowEndHeight), 1, height, color);
    }
}

#ifdef INCLUDE_DEBUG_MAP
void disasm_draw_branch_arrows(u32 printLine) {
    // Draw branch arrows from the buffer.
    struct BranchArrow* currArrow = &sBranchArrows[0];

    for (u32 i = 0; i < sNumBranchArrows; i++) {
        s32 startLine = (((s32)currArrow->startAddr - (s32)sDisasmViewportIndex) / DISASM_STEP);
        s32 endLine = (startLine + currArrow->branchOffset + 1);

        draw_branch_arrow(startLine, endLine, currArrow->xPos, sBranchColors[currArrow->colorIndex], printLine);

        currArrow++;
    }

    osWritebackDCacheAll();
}
#endif

static void print_as_insn(const u32 charX, const u32 charY, const Address addr, const Word data) {
    InsnData insn = { .raw = data };
    const char* destFname = NULL;
    const char* insnAsStr = insn_disasm(addr, insn, &destFname);

    // "[instruction name] [params]"
    crash_screen_print(charX, charY, "%s", insnAsStr);

#ifdef INCLUDE_DEBUG_MAP
    if (gCSSettings[CS_OPT_SYMBOL_NAMES].val && destFname != NULL) {
        // "[function name]"
        crash_screen_print_symbol_name_impl((charX + TEXT_WIDTH(INSN_NAME_DISPLAY_WIDTH)), charY,
            (CRASH_SCREEN_NUM_CHARS_X - (INSN_NAME_DISPLAY_WIDTH)),
            COLOR_RGBA32_CRASH_FUNCTION_NAME, destFname
        );
    }
#endif
}

static void print_as_binary(const u32 charX, const u32 charY, const Word data) { //! TODO: make this a custom formatting specifier?, maybe \%b?
    u32 bitCharX = charX;

    for (u32 c = 0; c < SIZEOF_BITS(Word); c++) {
        if ((c % SIZEOF_BITS(Byte)) == 0) { // Space between each byte.
            bitCharX += TEXT_WIDTH(1);
        }

        crash_screen_draw_glyph(bitCharX, charY, (((data >> (SIZEOF_BITS(Word) - c)) & 0b1) ? '1' : '0'), COLOR_RGBA32_WHITE);

        bitCharX += TEXT_WIDTH(1);
    }
}

static void disasm_draw_asm_entries(u32 line, u32 numLines, Address selectedAddr, Address pc) {
    u32 charX = TEXT_X(0);
    u32 charY = TEXT_Y(line);

    for (u32 y = 0; y < numLines; y++) {
        Address addr = (sDisasmViewportIndex + (y * DISASM_STEP));
        charY = TEXT_Y(line + y);

        // Draw crash and selection rectangles:
        if (addr == pc) {
            // Draw a red selection rectangle.
            crash_screen_draw_rect((charX - 1), (charY - 2), (CRASH_SCREEN_TEXT_W + 1), (TEXT_HEIGHT(1) + 1), COLOR_RGBA32_CRASH_PC_HIGHLIGHT);
            // "<-- CRASH"
            crash_screen_print((CRASH_SCREEN_TEXT_X2 - TEXT_WIDTH(STRLEN("<-- CRASH"))), charY, STR_COLOR_PREFIX"<-- CRASH", COLOR_RGBA32_CRASH_AT);
        } else if (addr == selectedAddr) {
            // Draw a gray selection rectangle.
            crash_screen_draw_row_selection_box(charY);
        }

        Word data = 0;
        if (!try_read_data(&data, addr)) {
            crash_screen_print(charX, charY, (STR_COLOR_PREFIX"*"), COLOR_RGBA32_CRASH_OUT_OF_BOUNDS);
        } else if (is_in_code_segment(addr)) {
            print_as_insn(charX, charY, addr, data);

            if ((addr == selectedAddr) && (gCSSettings[CS_OPT_DISASM_ARROW_MODE].val == DISASM_ARROW_MODE_SELECTION)) {
                InsnData insn = { .raw = data };
                s16 branchOffset = check_for_branch_offset(insn);

                if (branchOffset != 0x0000) {
                    draw_branch_arrow(y, (y + branchOffset + 1), DISASM_BRANCH_ARROW_OFFSET, sBranchColors[0], line);
                }
            }
        } else { // Outside of code segments:
            if (gCSSettings[CS_OPT_DISASM_BINARY].val) {
                // "bbbbbbbb bbbbbbbb bbbbbbbb bbbbbbbb"
                print_as_binary(charX, charY, data);
            } else {
                // "[XXXXXXXX]"
                crash_screen_print(charX, charY, STR_HEX_WORD, data);
            }
        }
    }

    osWritebackDCacheAll();
}

//! TODO: automatically check page/address change:
// Address sCurrFuncAddr = 0x00000000;
// const char* sCurrFuncName = NULL;

void disasm_draw(void) {
    __OSThreadContext* tc = &gCrashedThread->context;
    Address alignedSelectedAddr = ALIGNFLOOR(gSelectedAddress, DISASM_STEP);

    sDisasmBranchStartX = gCSSettings[CS_OPT_DISASM_OFFSET_ADDR].val
                        ? TEXT_X(INSN_NAME_DISPLAY_WIDTH + STRLEN("R0, R0, 0x80000000"))
                        : TEXT_X(INSN_NAME_DISPLAY_WIDTH + STRLEN("R0, R0, +0x0000"));
#ifdef INCLUDE_DEBUG_MAP
    sDisasmNumShownRows = gCSSettings[CS_OPT_DISASM_SHOW_SYMBOL].val ? 19 : 20;
#endif

    u32 line = 1;

    Address startAddr = sDisasmViewportIndex;
    Address endAddr   = (startAddr + ((sDisasmNumShownRows - 1) * DISASM_STEP));

    // "[XXXXXXXX] in [XXXXXXXX]-[XXXXXXXX]"
    crash_screen_print(TEXT_X(STRLEN("DISASM") + 1), TEXT_Y(line),
        (STR_COLOR_PREFIX STR_HEX_WORD" in "STR_HEX_WORD"-"STR_HEX_WORD),
        COLOR_RGBA32_WHITE, alignedSelectedAddr, startAddr, endAddr
    );

    line++;

#ifdef INCLUDE_DEBUG_MAP
    if (gCSSettings[CS_OPT_DISASM_SHOW_SYMBOL].val) {
        const struct MapSymbol* symbol = get_map_symbol(alignedSelectedAddr, SYMBOL_SEARCH_BACKWARD);
        if (symbol != NULL) {
            size_t charX = crash_screen_print(TEXT_X(0), TEXT_Y(line), "IN:");
            crash_screen_print_symbol_name(TEXT_X(charX), TEXT_Y(line), (CRASH_SCREEN_NUM_CHARS_X - charX), symbol);
        }

        line++;
    }

    if (gCSSettings[CS_OPT_DISASM_ARROW_MODE].val == DISASM_ARROW_MODE_FUNCTION) {
        disasm_draw_branch_arrows(line);
    }
#endif

    disasm_draw_asm_entries(line, sDisasmNumShownRows, alignedSelectedAddr, tc->pc);

    crash_screen_draw_divider(DIVIDER_Y(line));

    u32 line2 = (line + sDisasmNumShownRows);

    crash_screen_draw_divider(DIVIDER_Y(line2));

    u32 scrollTop = (DIVIDER_Y(line) + 1);
    u32 scrollBottom = DIVIDER_Y(line2);

    const size_t shownSection = ((sDisasmNumShownRows - 1) * DISASM_STEP);

    // Scroll bar:
    crash_screen_draw_scroll_bar(
        scrollTop, scrollBottom,
        shownSection, VIRTUAL_RAM_SIZE,
        (sDisasmViewportIndex - VIRTUAL_RAM_START),
        COLOR_RGBA32_CRASH_DIVIDER, TRUE
    );

    // Scroll bar crash position marker:
    crash_screen_draw_scroll_bar(
        scrollTop, scrollBottom,
        shownSection, VIRTUAL_RAM_SIZE,
        (tc->pc - VIRTUAL_RAM_START),
        COLOR_RGBA32_CRASH_AT, FALSE
    );

    osWritebackDCacheAll();
}

static void disasm_move_up(void) {
    gSelectedAddress = ALIGNFLOOR(gSelectedAddress, DISASM_STEP);
    // Scroll up.
    if (gSelectedAddress >= (VIRTUAL_RAM_START + DISASM_STEP))  {
        gSelectedAddress -= DISASM_STEP;
    }
}

static void disasm_move_down(void) {
    gSelectedAddress = ALIGNFLOOR(gSelectedAddress, DISASM_STEP);
    // Scroll down.
    if (gSelectedAddress <= (VIRTUAL_RAM_END - DISASM_STEP)) {
        gSelectedAddress += DISASM_STEP;
    }
}

void disasm_input(void) {
#ifdef INCLUDE_DEBUG_MAP
    Address oldPos = gSelectedAddress;
#endif

    if (gCSDirectionFlags.pressed.up) {
        disasm_move_up();
    }

    if (gCSDirectionFlags.pressed.down) {
        disasm_move_down();
    }

    u16 buttonPressed = gCSCompositeController->buttonPressed;

    if (buttonPressed & A_BUTTON) {
        open_address_select(get_branch_target_from_addr(gSelectedAddress));
    }

    sDisasmViewportIndex = clamp_view_to_selection(sDisasmViewportIndex, gSelectedAddress, sDisasmNumShownRows, DISASM_STEP);

#ifdef INCLUDE_DEBUG_MAP
    if (buttonPressed & B_BUTTON) {
        crash_screen_inc_setting(CS_OPT_SYMBOL_NAMES, TRUE);
    }

    if (gCSSettings[CS_OPT_DISASM_ARROW_MODE].val == DISASM_ARROW_MODE_FUNCTION) {
        //! TODO: don't reset branch buffer if switched page back into the same function.
        if (gCSSwitchedPage || (get_symbol_index_from_addr_forward(oldPos) != get_symbol_index_from_addr_forward(gSelectedAddress))) {
            gFillBranchBuffer = TRUE;
        }

        Address alignedSelectedAddress = ALIGNFLOOR(gSelectedAddress, DISASM_STEP);

        const struct MapSymbol* symbol = get_map_symbol(alignedSelectedAddress, SYMBOL_SEARCH_FORWARD);
        if (symbol != NULL) {
            const char* fname = get_map_symbol_name(symbol);

            if (gFillBranchBuffer) {
                gFillBranchBuffer = FALSE;
                reset_branch_buffer(symbol->addr);
                sContinueFillBranchBuffer = TRUE;
            }

            if (sContinueFillBranchBuffer) {
                sContinueFillBranchBuffer = disasm_fill_branch_buffer(fname, symbol->addr);
            }
        } else {
            gFillBranchBuffer = FALSE;
            reset_branch_buffer(alignedSelectedAddress);
            sContinueFillBranchBuffer = FALSE;
        }
    }
#endif
}
