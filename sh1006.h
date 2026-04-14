#ifndef MYSH1106_H
#define MYSH1106_H

#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SH1106_I2C_ADDR 0x3C
#define OLED_W 128
#define OLED_H 64

#define I2C_MACRO i2c_default //might want to change that according to your wiring

#define CMD_REGISTER 0x00
#define DATA_REGISTER 0x40

#define DISPLAY_OFF 0xAE
#define DISPLAY_ON 0xAF

#define LOWER_COL_ADDR 0x02 //the x beggining of the screen
#define HIGHER_COL_ADDR 0x10

#define PAGE_ADDR 0xB0
#define COM_DIRECTION 0xC8 //0xC0 for reversed vertically

#define START_LINE 0x40 // the y beggining of the screen

#define SET_CONTRAST_CONTROL_MODE 0x81

#define CONTRAST_VAL 0xFF //how bright is the oled

#define SEGMENT_REMAP 0xA1

#define SET_NORMAL_DISPLAY 0xA6
#define SET_REVERSE_DISPLAY 0xA7

#define SET_MULTIPLEX_RATION_MODE 0xA8
#define MULTIPLEX_RATION_VAL 0x3F // how many rows will display show

#define ENTIRE_DISPLAY_ON 0xA4
#define ENTIRE_DISPLAY_OFF 0xA5

#define SET_DISPLAY_OFFSET 0xD3
#define DISPLAY_OFFSET_VAL 0x00

#define SET_FREQUENCY_MODE 0xD5
#define FREQUENCY_VALUE 0xF0 //maybe change later to F0

#define SET_PRECHARGE_PERIOD_MODE 0xD9
#define PRECHARGE_PERIOD_VAL 0x22

#define SET_COM_HARDW_CONF_MODE 0xDA
#define COM_HARD_CONF_MODE 0x12 // 0x12 for 128x64 px 0x02 for 128x32 px

#define SET_VCOM_DESELECT_MODE 0xDB
#define VCOM_DESELECT_VALUE 0x20 //maybe change later to 35

#define SET_DC_CONTROL_MODE 0x8D
#define DC_DC_VAL 0x14

#define SCROLL_OFF 0x2E


typedef enum {BLACK, WHITE} Color;

typedef struct{
    uint8_t buffer[OLED_W * OLED_H / 8];
    uint8_t negative, inverted;
    Color color;
}SH1106;

void oled_write_register(uint8_t data){
    uint8_t src[2] = {CMD_REGISTER, data};

    i2c_write_blocking(I2C_MACRO, SH1106_I2C_ADDR, src, 2, false);
}

void oled_write_register_multi(uint8_t* data, uint16_t count){
    uint8_t src[OLED_W + 1];
    src[0] = DATA_REGISTER;
    for (uint16_t i = 0; i < count; i++){
        src[1 + i] = data[i];
    }
    i2c_write_blocking(I2C_MACRO, SH1106_I2C_ADDR, src, count + 1, false); // false not true
}

void oled_update_screen(SH1106* oled){
    for (uint8_t m = 0; m < 8; m++){
        oled_write_register(PAGE_ADDR + m);
        oled_write_register(LOWER_COL_ADDR);
        oled_write_register(HIGHER_COL_ADDR);

        oled_write_register_multi(&oled->buffer[OLED_W*m], OLED_W);
    }
}
/* Reversing black pixel to colored one and colored one to black and vise verse */
void oled_negative_screen(SH1106* oled){
    oled->negative = !oled->negative;
    if (oled->negative)
        oled_write_register(SET_NORMAL_DISPLAY);
    else
        oled_write_register(SET_REVERSE_DISPLAY);
}
/*Inverts screen vertically. Only vertically */
void oled_invert_screen(SH1106* oled){
    oled->inverted = !oled->inverted;
    if (oled->inverted)
        oled_write_register(0xC0);
    else
        oled_write_register(0xC8);
}

void oled_fill(Color color, SH1106* oled){
    memset(oled->buffer, (color == BLACK) ? 0x00 : 0xFF, sizeof(oled->buffer));
}

void oled_set_color(Color color, SH1106* oled){
    oled->color = color;
}

void oled_draw_pixel(uint8_t x, uint8_t y, SH1106* oled){
    if (x >= OLED_W || y >= OLED_H)
        return;

    if (oled->color == WHITE){
        oled->buffer[x + (y / 8) * OLED_W] |= 1 << (y % 8);
    } else {
        oled->buffer[x + (y / 8) * OLED_W] &= ~(1 << (y % 8));
    }
}

void oled_print_ch(char ch, uint8_t x, uint8_t y, const uint8_t* font, SH1106* oled){
    uint8_t ascii_pos = ch - 32;
    uint8_t font_width = 5;
    for (uint8_t col = 0; col < 5; col++){
        for (uint8_t row = 0; row < 7; row++){
            if (font[ascii_pos*5 + col] & (1 << row)) {
               oled_draw_pixel(x + col, y + row, oled);
            }
        }
        //oled->buffer[x + col + (y/8) * OLED_W] |= font[(ascii_pos * 5) + col];
    }
}

void oled_print_str(const char* str, uint8_t x, uint8_t y, const uint8_t* font, SH1106* oled){
    uint8_t spacing = 6;
    while (*str){
        oled_print_ch(*str++, x, y, font, oled);
        x += spacing;
    }
}


void oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106* oled){
    /*int8_t dx = end_x - start_x;
    int8_t dy = end_y - start_y;
    if (dx != 0 && dy != 0){
        float slope = (float)dy/(float)dx;
        for (uint8_t x = start_x; x <= end_x; x++){
            uint8_t y = slope * (x - start_x) + start_y;
            oled_draw_pixel(x, y, oled);
        }
    }
    else if (dx == 0){
        for (uint8_t y = start_y; y < end_y; y++)
            oled_draw_pixel(start_x, y, oled);
    }
    else if (dy == 0) {
        for (uint8_t x = start_x; x < end_x; x++)
            oled_draw_pixel(x, start_y, oled);
    }*/

    //Modified Bresenham’s algorithm
    int8_t slop = 2 * (y2 - y1);
    int8_t slop_error = slop - (x2 - x1);

    for (uint8_t x = x1, y = y1; x <= x2; x++){
        slop_error += slop;

        while (slop_error >=0){
            oled_draw_pixel(x, y, oled);
            y++;
            slop_error -= 2*(x2-x1);
        }
    }

}

SH1106 oled_init(){
    oled_write_register(DISPLAY_OFF);

    //oled_write_register(SET_VCOM_DESELECT_MODE);
    oled_write_register(VCOM_DESELECT_VALUE);
    oled_write_register(0x10);

    oled_write_register(PAGE_ADDR);

    oled_write_register(COM_DIRECTION);

    oled_write_register(LOWER_COL_ADDR);
    oled_write_register(HIGHER_COL_ADDR);

    oled_write_register(START_LINE);

    oled_write_register(SET_CONTRAST_CONTROL_MODE);
    oled_write_register(CONTRAST_VAL);

    oled_write_register(SEGMENT_REMAP);

    oled_write_register(SET_NORMAL_DISPLAY);

    oled_write_register(SET_MULTIPLEX_RATION_MODE);
    oled_write_register(MULTIPLEX_RATION_VAL);

    oled_write_register(ENTIRE_DISPLAY_ON);

    oled_write_register(SET_DISPLAY_OFFSET);
    oled_write_register(DISPLAY_OFFSET_VAL);

    oled_write_register(SET_FREQUENCY_MODE);
    oled_write_register(FREQUENCY_VALUE);

    oled_write_register(SET_PRECHARGE_PERIOD_MODE);
    oled_write_register(PRECHARGE_PERIOD_VAL);

    oled_write_register(SET_COM_HARDW_CONF_MODE);
    oled_write_register(COM_HARD_CONF_MODE);

    oled_write_register(SET_VCOM_DESELECT_MODE);
    oled_write_register(VCOM_DESELECT_VALUE);

    /*oled_write_register(SET_DC_CONTROL_MODE);
        oled_write_register(DC_DC_ON);*/
    oled_write_register(SET_DC_CONTROL_MODE);
    oled_write_register(DC_DC_VAL);

    oled_write_register(SCROLL_OFF);

    oled_write_register(DISPLAY_ON);

    SH1106 oled;

    oled.color = WHITE;
    oled.negative = 0;

    oled_fill(BLACK, &oled);
    oled_update_screen(&oled);

    return oled;
}

#endif
