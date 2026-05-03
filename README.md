# SH1106-driver-rpi-pico
SH1106 driver for 1,3 inch OLED display 128x64 px for rpi pico series\
In example folder you can find how to use the driver
### Features
* Pixel-level drawing
* Line drawing between any two points (Bresenham's algorithm)
* Formatted text printing (printf-style)
* Bitmap rendering

## Setup
In `CMakeLists.txt` add `hardware_i2c` to your `target_link_libraries`:
```
target_link_libraries(your_target
    pico_stdlib
    hardware_i2c
)
```
Perform standard I2C initialization before using the driver:
```c
i2c_init(i2c_default, 400000);
gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
```
Initialize display
```c
oled_init();
```
All drawing is written to an internal framebuffer, so after you finish drawing, call `oled_update_screen()` to push the framebuffer to the display:
```c
oled_update_screen()
```
## API Reference
### Core
```c
// Initializes the display, perfomes primary screen clearing and setting up the screen to work properly.
void oled_init()
// Write framebuffer data to the display.
void oled_update_screen()
```
### Screen Control
```c
// Invert display so top becomes bottom left becomes right and so on.
void oled_invert_screen()

// Negate every pixel. Black pixels become white and the white pixels become black.
void oled_negative_screen();
```
### Drawing
```c
// The color enum
typedef enum {BLACK, WHITE} Color;

// Set color
void oled_set_color(Color color)

// Fills screen with chosen color
void oled_fill(Color color);

// Draw single pixel at x y coordinates counting from top-left corner
void oled_draw_pixel(uint8_t x, uint8_t y);

// Draw bitmap. width and height parametrs must be the same as bitmap dimension, othrwise drawing will be disordered 
void oled_draw_bitmap(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* bitmap);

// Draw horizontal line
void oled_draw_hline(uint8_t x, uint8_t y, uint8_t width);

// Draw vertical line
void oled_draw_vline(uint8_t x, uint8_t y, uint8_t height);

// Draw line from any x1 y1 to any x2 y2
void oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

// Draw filled rectangle
void oled_draw_rect_filled(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

// Draw rectangle outline
void oled_draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
```
### Printing 
```c
// Set 5x7 font (the default one)
void oled_set_font5x7();

// Set smaller 3x5 font
void oled_set_font3x5();

// Print single character. Fonts can be found in fonts.h
void oled_print_ch(char ch, uint8_t x, uint8_t y, const uint8_t* font);

// Print string
void oled_print_str(const char* str, uint8_t x, uint8_t y, const uint8_t* font);

// Print int. _digits is formatting parametr
void oled_print_int(int val, uint8_t _digits, uint8_t x, uint8_t y, const uint8_t* font);

// Print float with dig_sep digits after decimal separation
void oled_print_float(float val, uint8_t dig_sep, uint8_t x, uint8_t y, const uint8_t* font);

// Print string formatting. ... - your formats as in printf() function
void oled_print_str_formating(const char* str, uint8_t x, uint8_t y, const uint8_t* font, ...);
```
Note:
```c
//Before printing with 3x5 font, you need to call oled_set_font3x5() to set font and after this call printing function

oled_set_font3x5();
oled_print_str("My text", 0, 0, font3x5);

// And if you want to go back to the 5x7 font, you'll need to set font back

oled_set_font5x7();
oled_print_str("My other text", 0, 10, font5x7);
```
