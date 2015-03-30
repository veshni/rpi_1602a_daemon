#include "rpi_1602_gpio.h"
#include <glib.h>

#define PIN_RS 		25
#define PIN_E 		24
#define PIN_DB_0	23
#define PIN_DB_1	17
#define PIN_DB_2	21
#define PIN_DB_3	22

static gint numlines = 1;
static guint displayfunction = 0x0;
static guint displaycontrol = 0x0;
static guint displaymode = 0x0;

static void write_4_bits(char bits, gboolean char_mode);
static void pulse_enable();
static void write_message(char* text);
static void set_gpio(guint pin, gboolean value);

void begin(gint lines)
{
	if (lines > 1) {
        numlines = lines;
        displayfunction |= LCD_2LINE;
	}
}

void home()
{
    write_4_bits(LCD_RETURNHOME, FALSE);
    g_usleep(3000);
}

void clear()
{
    write_4_bits(LCD_CLEARDISPLAY, FALSE);
    g_usleep(3000);
}

void set_cursor(guint col, guint row)
{
    guint row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > numlines) {
        row = numlines - 1;
    }
    write_4_bits(LCD_SETDDRAMADDR | (col + row_offsets[row]), FALSE);
}

void no_display()
{
    displaycontrol &= ~LCD_DISPLAYON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol, FALSE);
}

void display()
{
    displaycontrol |= LCD_DISPLAYON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol, FALSE);
}

void no_cursor()
{
    displaycontrol &= ~LCD_CURSORON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol, FALSE);
}

void cursor()
{
    displaycontrol |= LCD_CURSORON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol, FALSE);
}

void no_blink()
{
    displaycontrol &= ~LCD_BLINKON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol, FALSE);
}

void blink()
{
    displaycontrol |= LCD_BLINKON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol, FALSE);
}

void display_left()
{
    write_4_bits(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT, FALSE);
}

void scroll_display_right()
{
    write_4_bits(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT, FALSE);
}

void left_to_right()
{
    displaymode |= LCD_ENTRYLEFT;
    write_4_bits(LCD_ENTRYMODESET | displaymode, FALSE);
}

void right_to_left()
{
    displaymode &= ~LCD_ENTRYLEFT;
    write_4_bits(LCD_ENTRYMODESET | displaymode, FALSE);
}

void autoscroll()
{
    displaymode |= LCD_ENTRYSHIFTINCREMENT;
    write_4_bits(LCD_ENTRYMODESET | displaymode, FALSE);
}

void no_autoscroll()
{
    displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    write_4_bits(LCD_ENTRYMODESET | displaymode, FALSE);
}

static void write_4_bits(char bits, gboolean char_mode)
{
    g_usleep(1000);

    set_gpio(PIN_RS, char_mode);
    
    set_gpio(PIN_DB_0, FALSE);
    set_gpio(PIN_DB_1, FALSE);
    set_gpio(PIN_DB_2, FALSE);
    set_gpio(PIN_DB_3, FALSE);
   
    set_gpio(PIN_DB_0, bits & 1 ? TRUE : FALSE);
    set_gpio(PIN_DB_1, bits & 1 << 1 ? TRUE : FALSE);
    set_gpio(PIN_DB_2, bits & 1 << 2 ? TRUE : FALSE);
    set_gpio(PIN_DB_3, bits & 1 << 3 ? TRUE : FALSE);

    pulse_enable();

    set_gpio(PIN_DB_0, FALSE);
    set_gpio(PIN_DB_1, FALSE);
    set_gpio(PIN_DB_2, FALSE);
    set_gpio(PIN_DB_3, FALSE);
   
    set_gpio(PIN_DB_0, bits & 1 << 4 ? TRUE : FALSE);
    set_gpio(PIN_DB_1, bits & 1 << 5 ? TRUE : FALSE);
    set_gpio(PIN_DB_2, bits & 1 << 6 ? TRUE : FALSE);
    set_gpio(PIN_DB_3, bits & 1 << 7 ? TRUE : FALSE);

    pulse_enable();
}

static void pulse_enable()
{
	set_gpio(PIN_E, FALSE);
    g_usleep(1);
    set_gpio(PIN_E, TRUE);
    g_usleep(1);
    set_gpio(PIN_E, FALSE);
    g_usleep(1);
}

static void write_message(char* text)
{
	size_t i;
	size_t word_len = strlen(text);
    for(i = 0; i < word_len; i++) {
        if (text[i] == '\n') {
            write_4_bits(0xC0, FALSE);
        }
        else {
            write_4_bits(text[i], TRUE);
        }
    }
}

static void set_gpio(guint pin, gboolean value) {

}

void rpi_1602_gpio_init()
{
   
    write_4_bits(0x33, FALSE);
    write_4_bits(0x32, FALSE);
    write_4_bits(0x28, FALSE);
    write_4_bits(0x0C, FALSE);
    write_4_bits(0x06, FALSE);

    displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS | LCD_2LINE;
    displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    write_4_bits(LCD_ENTRYMODESET | displaymode, FALSE);
    clear();
}

void rpi_1602_gpio_update(char *message) {
	log_err("Message is: %s", message);
	write_message(message);
}