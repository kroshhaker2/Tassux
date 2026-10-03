#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "../drivers/keyboard.h"
#include "../include/input-event-codes.h"
#include "../include/stdint.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Цвета VGA
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHT_GREY 7
#define DARK_GREY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA 13
#define LIGHT_BROWN 14
#define WHITE 15

#define DEFAULT_TEXT_COLOR LIGHT_GREY

struct vga_char
{
    char character;
    char attribute;
} __attribute__((packed));

#define CONSOLE_BUFFER_SIZE 128

typedef struct
{
    char buf[CONSOLE_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
} console_buffer_t;

#define MOD_SHIFT (1 << 0)
#define MOD_CTRL (1 << 1)
#define MOD_ALT (1 << 2)

extern console_buffer_t kb_buffer;
extern void console_on_key(const key_event_t *);
extern void clear_screen(void);
extern void putchar(char c);
extern void print(const char *str);
extern void print_colored(const char *str, unsigned char color);
extern void print_hex(unsigned int num);
extern void print_hex8(uint8_t num);
extern void print_hex_buffer(const unsigned char *buf, unsigned int len);
extern void print_num(int num);
extern void print_num_width(uint32_t num, int width);
extern void printf(const char *format, ...);
extern void println(const char *str);
extern void scroll(void);
extern void set_cursor(int x, int y);
void console_init(void);

// keymap для обычных клавиш
static const char keymap[128] = {
    [KEY_1] = '1',          [KEY_2] = '2',         [KEY_3] = '3',           [KEY_4] = '4',
    [KEY_5] = '5',          [KEY_6] = '6',         [KEY_7] = '7',           [KEY_8] = '8',
    [KEY_9] = '9',          [KEY_0] = '0',         [KEY_MINUS] = '-',       [KEY_EQUAL] = '=',
    [KEY_Q] = 'q',          [KEY_W] = 'w',         [KEY_E] = 'e',           [KEY_R] = 'r',
    [KEY_T] = 't',          [KEY_Y] = 'y',         [KEY_U] = 'u',           [KEY_I] = 'i',
    [KEY_O] = 'o',          [KEY_P] = 'p',         [KEY_LEFTBRACE] = '[',   [KEY_RIGHTBRACE] = ']',
    [KEY_A] = 'a',          [KEY_S] = 's',         [KEY_D] = 'd',           [KEY_F] = 'f',
    [KEY_G] = 'g',          [KEY_H] = 'h',         [KEY_J] = 'j',           [KEY_K] = 'k',
    [KEY_L] = 'l',          [KEY_SEMICOLON] = ';', [KEY_APOSTROPHE] = '\'', [KEY_GRAVE] = '`',
    [KEY_BACKSLASH] = '\\', [KEY_Z] = 'z',         [KEY_X] = 'x',           [KEY_C] = 'c',
    [KEY_V] = 'v',          [KEY_B] = 'b',         [KEY_N] = 'n',           [KEY_M] = 'm',
    [KEY_COMMA] = ',',      [KEY_DOT] = '.',       [KEY_SLASH] = '/',       [KEY_SPACE] = ' ',
    [KEY_ENTER] = '\n',     [KEY_TAB] = '\t'};

// keymap для клавиш с Shift
static const char keymap_shift[128] = {
    [KEY_1] = '!',         [KEY_2] = '@',         [KEY_3] = '#',          [KEY_4] = '$',
    [KEY_5] = '%',         [KEY_6] = '^',         [KEY_7] = '&',          [KEY_8] = '*',
    [KEY_9] = '(',         [KEY_0] = ')',         [KEY_MINUS] = '_',      [KEY_EQUAL] = '+',
    [KEY_Q] = 'Q',         [KEY_W] = 'W',         [KEY_E] = 'E',          [KEY_R] = 'R',
    [KEY_T] = 'T',         [KEY_Y] = 'Y',         [KEY_U] = 'U',          [KEY_I] = 'I',
    [KEY_O] = 'O',         [KEY_P] = 'P',         [KEY_LEFTBRACE] = '{',  [KEY_RIGHTBRACE] = '}',
    [KEY_A] = 'A',         [KEY_S] = 'S',         [KEY_D] = 'D',          [KEY_F] = 'F',
    [KEY_G] = 'G',         [KEY_H] = 'H',         [KEY_J] = 'J',          [KEY_K] = 'K',
    [KEY_L] = 'L',         [KEY_SEMICOLON] = ':', [KEY_APOSTROPHE] = '"', [KEY_GRAVE] = '~',
    [KEY_BACKSLASH] = '|', [KEY_Z] = 'Z',         [KEY_X] = 'X',          [KEY_C] = 'C',
    [KEY_V] = 'V',         [KEY_B] = 'B',         [KEY_N] = 'N',          [KEY_M] = 'M',
    [KEY_COMMA] = '<',     [KEY_DOT] = '>',       [KEY_SLASH] = '?',      [KEY_SPACE] = ' ',
    [KEY_ENTER] = '\n',    [KEY_TAB] = '\t'};

#endif