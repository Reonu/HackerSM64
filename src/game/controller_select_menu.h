#pragma once

#include <PR/ultratypes.h>

#include "types.h"


struct ButtonName {
    /*0x00*/ u8 pad[2];
    /*0x02*/ u16 mask;              // Button mask.
    /*0x04*/ const char* name;      // Button name string.
}; /*0x08*/

typedef union {
    struct PACKED {
        /*0x00*/ u16 mask;          // Button mask.
        /*0x02*/ u8 x : 5, w : 3;   // [0..31], [0..7]
        /*0x03*/ u8 y : 5, h : 3;   // [0..31], [0..7]
    }; /*0x04*/
    u32 raw;
} ButtonHighlight; /*0x04*/

struct ControllerIcon {
    /*0x00*/ u8 pad[2];
    /*0x02*/ u16 type;              // Controller type.
    /*0x04*/ Texture* texture;      // Pointer to the controller texture for the above type.
#ifdef CONTROLLERS_INPUT_DISPLAY
    /*0x08*/ const ButtonHighlight (*buttonHighlightList)[];  // Pointer to the list of button highlights for the above texture.
#endif // CONTROLLERS_INPUT_DISPLAY
}; /*0x0C*/


#ifdef ENABLE_STATUS_REPOLLING_GUI
void render_controllers_overlay(void);
#else
#define render_controllers_overlay()
#endif
