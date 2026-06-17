#include "console.h"

#include "../include/convert.h"
#include "../include/ctype.h"
#include "../include/io.h"
#include "command.h"

static struct vga_char *vga_buffer = (struct vga_char *)VGA_MEMORY;
static int cursor_x = 0, cursor_y = 0;

console_buffer_t kb_buffer = {0};

char keycode_to_ascii(uint16_t key, uint8_t mods)
{
    if (mods & MOD_SHIFT)
        return keymap_shift[key];
    return keymap[key];
}

void console_buffer_put(console_buffer_t *cb, char c)
{
    uint8_t next = (cb->head + 1) % CONSOLE_BUFFER_SIZE;
    if (next != cb->tail)
    {
        cb->buf[cb->head] = c;
        cb->head = next;
    }
}

void console_buffer_backspace(console_buffer_t *cb)
{
    if (cb->head != cb->tail)
    {
        if (cb->head == 0)
            cb->head = CONSOLE_BUFFER_SIZE - 1;
        else
            cb->head--;
    }
}

char console_buffer_get(console_buffer_t *cb)
{
    if (cb->head == cb->tail)
        return 0;
    char c = cb->buf[cb->tail];
    cb->tail = (cb->tail + 1) % CONSOLE_BUFFER_SIZE;
    return c;
}

void console_on_key(const key_event_t *ev)
{
    if (!ev->pressed)
        return;

    if (ev->mods & MOD_CTRL)
    {
        if (ev->keycode == KEY_C)
        {
            // console_signal(SIGINT); // TODO: сигнал для прерывания
            return;
        }
    }

    if (ev->keycode == KEY_UP)
    {
        print("\x1B[A");
        return;
    }
    else if (ev->keycode == KEY_DOWN)
    {
        print("\x1B[B");
        return;
    }

    if (ev->keycode == KEY_BACKSPACE)
    {
        console_buffer_backspace(&kb_buffer);
        putchar('\b'); // смещение курсора
        putchar(' ');  // затираем символ
        putchar('\b'); // возвращаем курсор
        return;
    }

    // Enter
    if (ev->keycode == KEY_ENTER)
    {
        putchar('\n');
        parse_command(kb_buffer.buf);
        kb_buffer.tail = kb_buffer.head;
        return;
    }

    char ch = keycode_to_ascii(ev->keycode, ev->mods);
    if (ch)
    {
        // console_buffer_put(&kb_buffer, ch);
        putchar(ch);
    }
}

void console_process_input(void)
{
    char c = console_buffer_get(&kb_buffer);
    if (c)
    {
        // Выводим символ на экран или передаем в парсер
        putchar(c);
    }
}

void clear_screen(void)
{
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            vga_buffer[y * VGA_WIDTH + x].character = ' ';
            vga_buffer[y * VGA_WIDTH + x].attribute = DEFAULT_TEXT_COLOR | (BLACK << 4);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void set_cursor(int x, int y)
{
    unsigned short pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void scroll(void)
{
    for (int y = 0; y < VGA_HEIGHT - 1; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }

    for (int x = 0; x < VGA_WIDTH; x++)
    {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x].character = ' ';
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x].attribute = DEFAULT_TEXT_COLOR | (BLACK << 4);
    }

    cursor_y = VGA_HEIGHT - 1;
}

void putchar(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
    }
    else if (c == '\r')
    {
        cursor_x = 0;
    }
    else if (c == '\b')
    {
        if (cursor_x > 0)
        {
            cursor_x--;
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x].character = ' ';
        }
    }
    else if (c == '\t')
    {
        cursor_x = (cursor_x + 8) & ~(8 - 1);
    }
    else
    {
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x].character = c;
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x].attribute = DEFAULT_TEXT_COLOR | (BLACK << 4);
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH)
    {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= VGA_HEIGHT)
    {
        scroll();
    }

    set_cursor(cursor_x, cursor_y);
}

void print(const char *str)
{
    while (*str)
    {
        putchar(*str);
        str++;
    }
}

void println(const char *str)
{
    while (*str)
    {
        putchar(*str);
        str++;
    }
    putchar('\n');
}

void printf(const char *format, ...)
{
    char **arg = (char **)&format;
    int c;
    char buf[20];

    arg++;

    while ((c = *format++) != 0)
    {
        if (c != '%')
            putchar(c);
        else
        {
            char *p, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0')
            {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9')
            {
                pad = c - '0';
                c = *format++;
            }

            switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
                itoa(buf, c, *((int *)arg++));
                p = buf;
                goto string;
                break;

            case 's':
                p = *arg++;
                if (!p)
                    p = "(null)";

            string:
                for (p2 = p; *p2; p2++)
                    ;
                for (; p2 < p + pad; p2++)
                    putchar(pad0 ? '0' : ' ');
                while (*p)
                    putchar(*p++);
                break;

            default:
                putchar(*((int *)arg++));
                break;
            }
        }
    }
}

void print_colored(const char *str, unsigned char color)
{
    int old_x = cursor_x, old_y = cursor_y;
    while (*str)
    {
        if (*str == '\n')
        {
            cursor_x = 0;
            cursor_y++;
        }
        else
        {
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x].character = *str;
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x].attribute = color;
            cursor_x++;
        }

        if (cursor_x >= VGA_WIDTH)
        {
            cursor_x = 0;
            cursor_y++;
        }

        if (cursor_y >= VGA_HEIGHT)
        {
            scroll();
        }

        str++;
    }
    set_cursor(cursor_x, cursor_y);
}

void print_hex(unsigned int num)
{
    print("0x");
    char hex_digits[] = "0123456789ABCDEF";

    for (int i = 28; i >= 0; i -= 4)
    {
        putchar(hex_digits[(num >> i) & 0xF]);
    }
}

void print_hex8(uint8_t num)
{
    char hex[3];
    const char *digits = "0123456789ABCDEF";
    hex[0] = digits[(num >> 4) & 0x0F];
    hex[1] = digits[num & 0x0F];
    hex[2] = '\0';

    print(hex);
}
void print_hex_buffer(const unsigned char *buf, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        unsigned char byte = buf[i];

        char hex[3];
        const char *digits = "0123456789ABCDEF";
        hex[0] = digits[(byte >> 4) & 0x0F];
        hex[1] = digits[byte & 0x0F];
        hex[2] = '\0';

        print(hex);

        print(" ");

        if ((i + 1) % 26 == 0)
            print("\n");
    }
}

void print_num(int num)
{
    if (num == 0)
    {
        putchar('0');
        return;
    }

    if (num < 0)
    {
        putchar('-');
        num = -num;
    }

    char buffer[32];
    int i = 0;

    while (num > 0)
    {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    for (int j = i - 1; j >= 0; j--)
    {
        putchar(buffer[j]);
    }
}

void print_num_width(uint32_t num, int width)
{
    char buf[12];
    i2s(num, buf);
    int len = 0;
    while (buf[len] != '\0')
        len++;

    for (int i = 0; i < width - len; i++)
        putchar(' ');

    print(buf);
}

void console_init(void)
{
    clear_screen();

    print_colored("=====================================\n", LIGHT_CYAN | (BLACK << 4));
    print_colored("           Welcome to Tassux         \n", WHITE | (BLACK << 4));
    print_colored("=====================================\n", LIGHT_CYAN | (BLACK << 4));
    print_colored("Version: 4\n\n", GREEN);

    print("\nType 'help' for a list of commands\n\n");
}
