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

static void begin(gint lines)
{
	if (lines > 1) {
        numlines = lines;
        displayfunction |= LCD_2LINE;
	}
}

static void home()
{
    write_4_bits(LCD_RETURNHOME);
    g_usleep(3000);
}

static void clear()
{
    write_4_bits(LCD_CLEARDISPLAY);
    g_usleep(3000);
}

static void set_cursor(guint col, guint row)
{
    guint row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > numlines) {
        row = numlines - 1;
    }
    write_4_bits(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

static void no_display()
{
    displaycontrol &= ~LCD_DISPLAYON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol);
}

static void display()
{
    displaycontrol |= LCD_DISPLAYON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol);
}

static void no_cursor()
{
    displaycontrol &= ~LCD_CURSORON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol);
}

static void cursor()
{
    displaycontrol |= LCD_CURSORON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol);
}

static void no_blink()
{
    displaycontrol &= ~LCD_BLINKON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol);
}

static void blink()
{
    displaycontrol |= LCD_BLINKON;
    write_4_bits(LCD_DISPLAYCONTROL | displaycontrol);
}

static void display_left()
{
    write_4_bits(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

static void scroll_display_right()
{
    write_4_bits(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

static void left_to_right()
{
    displaymode |= LCD_ENTRYLEFT;
    write_4_bits(LCD_ENTRYMODESET | displaymode);
}

static void right_to_left()
{
    displaymode &= ~LCD_ENTRYLEFT;
    write_4_bits(LCD_ENTRYMODESET | displaymode);
}

static void autoscroll()
{
    displaymode |= LCD_ENTRYSHIFTINCREMENT;
    write_4_bits(LCD_ENTRYMODESET | displaymode);
}

static void no_autoscroll()
{
    displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    write_4_bits(LCD_ENTRYMODESET | displaymode);
}

static void write_4_bits(char bits, gboolean char_mode)
{
    g_usleep(1000);

    set_gpio(PIN_RS, char_mode);
    
    set_gpio(PIN_DB_0, false);
    set_gpio(PIN_DB_1, false);
    set_gpio(PIN_DB_2, false);
    set_gpio(PIN_DB_3, false);
   
    set_gpio(PIN_DB_0, bits & 1 ? true : false);
    set_gpio(PIN_DB_1, bits & 1 << 1 ? true : false);
    set_gpio(PIN_DB_2, bits & 1 << 2 ? true : false);
    set_gpio(PIN_DB_3, bits & 1 << 3 ? true : false);

    pulse_enable()

    set_gpio(PIN_DB_0, false);
    set_gpio(PIN_DB_1, false);
    set_gpio(PIN_DB_2, false);
    set_gpio(PIN_DB_3, false);
   
    set_gpio(PIN_DB_0, bits & 1 << 4 ? true : false);
    set_gpio(PIN_DB_1, bits & 1 << 5 ? true : false);
    set_gpio(PIN_DB_2, bits & 1 << 6 ? true : false);
    set_gpio(PIN_DB_3, bits & 1 << 7 ? true : false);

    pulse_enable()
}

static void pulse_enable()
{
	set_gpio(PIN_E, false);
    g_usleep(1);
    set_gpio(PIN_E, true);
    g_usleep(1);
    set_gpio(PIN_E, false);
    g_usleep(1);
}

static void message(char* text)
{
	size_t word_len = strlen(text);
    for(i = 0; i < word_len; i++) {
        if (text[i] == '\n') {
            write_4_bits(0xC0);
        }
        else {
            write_4_bits(text[i], true);
        }
    }
}

static void set_gpio(guint pin, gboolean value) {

}

void rpi_1602_gpio_init()
{
   
    write_4_bits(0x33);
    write_4_bits(0x32);
    write_4_bits(0x28);
    write_4_bits(0x0C);
    write_4_bits(0x06);

    displaycontrol = self.LCD_DISPLAYON | self.LCD_CURSOROFF | self.LCD_BLINKOFF;
    displayfunction = self.LCD_4BITMODE | self.LCD_1LINE | self.LCD_5x8DOTS | self.LCD_2LINE;
    displaymode = self.LCD_ENTRYLEFT | self.LCD_ENTRYSHIFTDECREMENT;
    write_4_bits(self.LCD_ENTRYMODESET | self.displaymode);
    clear();
}

void rpi_1602_gpio_update(char *message) {
	log_err("Message is: %s", message);
}