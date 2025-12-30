#include "keyboard.h"

#include "../console/console.h"
#include "../include/stdbool.h"

static uint8_t mods = 0;

void keyboard_irq()
{
    uint8_t sc = inb(0x60);

    // bool released = sc & 0x80;
    // sc &= 0x7F;

    // uint16_t key = scancode_to_keycode[sc];

    // if (key == KEY_LEFTSHIFT || key == KEY_RIGHTSHIFT)
    //     released ? (mods &= ~MOD_SHIFT) : (mods |= MOD_SHIFT);

    // if (key == KEY_LEFTCTRL)
    //     released ? (mods &= ~MOD_CTRL) : (mods |= MOD_CTRL);

    // key_event_t ev = {.keycode = key, .pressed = !released, .mods = mods};

    print_hex8(sc);
    // console_on_key(&ev);
}
