#ifndef RPI_1602_GPIO
#define RPI_1602_GPIO

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <glib.h>

#define log_err(...) syslog(LOG_ERR, __VA_ARGS__)
#define log_dbg(...) syslog(LOG_NOTICE, __VA_ARGS__) 

#define LCD_CLEARDISPLAY         0x01
#define LCD_RETURNHOME           0x02
#define LCD_ENTRYMODESET         0x04
#define LCD_DISPLAYCONTROL       0x08
#define LCD_CURSORSHIFT          0x10
#define LCD_FUNCTIONSET          0x20
#define LCD_SETCGRAMADDR         0x40
#define LCD_SETDDRAMADDR         0x80

#define LCD_ENTRYRIGHT           0x00
#define LCD_ENTRYLEFT            0x02
#define LCD_ENTRYSHIFTINCREMENT  0x01
#define LCD_ENTRYSHIFTDECREMENT  0x00

#define LCD_DISPLAYON            0x04
#define LCD_DISPLAYOFF           0x00
#define LCD_CURSORON             0x02
#define LCD_CURSOROFF            0x00
#define LCD_BLINKON              0x01
#define LCD_BLINKOFF             0x00
#define LCD_DISPLAYMOVE          0x08
#define LCD_CURSORMOVE           0x00

#define LCD_DISPLAYMOVE          0x08
#define LCD_CURSORMOVE           0x00
#define LCD_MOVERIGHT            0x04
#define LCD_MOVELEFT             0x00

#define LCD_8BITMODE             0x10
#define LCD_4BITMODE             0x00
#define LCD_2LINE                0x08
#define LCD_1LINE                0x00
#define LCD_5x10DOTS             0x04
#define LCD_5x8DOTS              0x00

void rpi_1602_gpio_update(char *message);
void rpi_1602_gpio_init();
void begin(gint lines);
void home();
void clear();
void set_cursor(guint col, guint row);
void no_display();
void display();
void no_cursor();
void cursor();
void no_blink();
void blink();
void display_left();
void scroll_display_right();
void left_to_right();
void right_to_left();
void autoscroll();
void no_autoscroll();
#endif