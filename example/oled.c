#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"

#include "sh1006.h"
#include "fonts.h"


int main() {
    stdio_init_all();

    i2c_init(i2c_default, 400000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    SH1106 oled = oled_init();

    oled_draw_pixel(10, 10, &oled);

    oled_print_str("\"Helloworld\"(print)", 0, 5, font5x7, &oled);

    oled_draw_line(80, 10, 88, 30, &oled);

    oled_update_screen(&oled);
    while (true) {
        sleep_ms(1000);
    }
}
