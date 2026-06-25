#ifndef HW_BUTTONS_H
#define HW_BUTTONS_H

#include <stdint.h>

typedef enum
{
    BTN_NONE = 0,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_FIRE,
    BTN_COLOR
} ButtonEvent_t;

extern volatile ButtonEvent_t g_buttonEvent;

#endif