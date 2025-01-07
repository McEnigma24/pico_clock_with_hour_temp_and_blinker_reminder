#include "__preprocessor__.h"
#include "pico/stdlib.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "hardware/i2c.h"
#include "ssd1306.h"
#ifdef __cplusplus
}
#endif

int pico_led_init(void)
{
    #if defined(PICO_DEFAULT_LED_PIN)
        gpio_init(PICO_DEFAULT_LED_PIN);                // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);   // so we can use normal GPIO functionality to turn the led on and off
        return PICO_OK;
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        return cyw43_arch_init(); // For Pico W devices we need to initialise the driver etc
    #endif
}
void pico_set_led(bool led_on)
{
    #if defined(PICO_DEFAULT_LED_PIN)
        gpio_put(PICO_DEFAULT_LED_PIN, led_on); // Just set the GPIO on or off
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on); // Ask the wifi "driver" to set the GPIO on or off
    #endif
}

#define TIME_QUICK      ( 100 )
#define TIME_LONG       ( 250 )

void blink_quick(int num_of_blinks)
{
    for(int i=0; i<num_of_blinks; i++)
    {
        pico_set_led(true);
        sleep_ms(TIME_QUICK);
        pico_set_led(false);
        sleep_ms(TIME_QUICK);
    }
}
void blink_long(int num_of_blinks)
{
    for(int i=0; i<num_of_blinks; i++)
    {
        pico_set_led(true);
        sleep_ms(TIME_LONG);
        pico_set_led(false);
        sleep_ms(TIME_LONG);
    }
}

#define PIN_BUTTON_power 14
#define PIN_BUTTON_output 15

void button_init()
{
    gpio_init(PIN_BUTTON_power);
    gpio_set_dir(PIN_BUTTON_power, GPIO_OUT);
    gpio_put(PIN_BUTTON_power, 1); // Włączamy zasilanie na pinie (3.3V)
    
    gpio_init(PIN_BUTTON_output);
    gpio_set_dir(PIN_BUTTON_output, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_output);
}
bool button_clicked()
{
    return gpio_get(PIN_BUTTON_output) == 1;
}



// #define OTHER

#ifdef OTHER
    // Definicje pinów I2C
    #define I2C_PORT i2c0
    #define PIN_SDA 16
    #define PIN_SCL 17

    #define OLED_ADDR 0x3C // Typowy adres I2C dla SH1106

    // Prosta czcionka bitmapowa 5x8 (ASCII 32-126)
    static const uint8_t font_default[95][5] = {
        {0x00, 0x00, 0x00, 0x00, 0x00}, // Spacja
        {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
        {0x00, 0x07, 0x00, 0x07, 0x00}, // "
        // Dodaj tutaj resztę znaków...
    };

    // Funkcja wysyłająca polecenia do OLED
    void oled_send_command(uint8_t cmd) {
        uint8_t buf[2] = {0x00, cmd}; // 0x00 = tryb polecenia
        i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
    }

    // Funkcja inicjalizująca OLED
    void oled_init() {
        static const uint8_t init_commands[] = {
            0xAE,       // Display OFF
            0xD5, 0x80, // Set Display Clock Divide Ratio/Oscillator Frequency
            0xA8, 0x3F, // Set Multiplex Ratio
            0xD3, 0x00, // Set Display Offset
            0x40,       // Set Display Start Line
            0x8D, 0x14, // Enable charge pump
            0x20, 0x00, // Set Memory Addressing Mode to Horizontal
            0xA1,       // Set Segment Re-map
            0xC8,       // Set COM Output Scan Direction
            0xDA, 0x12, // Set COM Pins Hardware Configuration
            0x81, 0xCF, // Set Contrast Control
            0xD9, 0xF1, // Set Pre-charge Period
            0xDB, 0x40, // Set VCOMH Deselect Level
            0xA4,       // Entire Display ON (resume)
            0xA6,       // Set Normal Display (A7 for inverse)
            0xAF        // Display ON
        };

        for (uint8_t i = 0; i < sizeof(init_commands); i++) {
            oled_send_command(init_commands[i]);
        }
    }

    // Funkcja czyszcząca ekran OLED
    void oled_clear() {
        for (uint8_t page = 0; page < 8; page++) {
            oled_send_command(0xB0 + page); // Ustawienie strony
            oled_send_command(0x00);       // Ustawienie kolumny (niski bajt)
            oled_send_command(0x10);       // Ustawienie kolumny (wysoki bajt)
            for (uint8_t col = 0; col < 128; col++) {
                uint8_t buf[2] = {0x40, 0x00}; // 0x40 = tryb danych
                i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
            }
        }
    }

    // Funkcja rysująca znak na OLED
    void oled_draw_char(uint8_t x, uint8_t y, char c) {
        oled_send_command(0xB0 + y);       // Ustawienie strony
        oled_send_command(0x00 + (x & 0x0F)); // Ustawienie kolumny (niski bajt)
        oled_send_command(0x10 + (x >> 4));   // Ustawienie kolumny (wysoki bajt)

        for (uint8_t i = 0; i < 5; i++) {
            uint8_t buf[2] = {0x40, font_default[c - 32][i]};
            i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
        }

        // Dodaj odstęp między znakami
        uint8_t buf[2] = {0x40, 0x00};
        i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
    }

    // Funkcja rysująca ciąg znaków na OLED
    void oled_draw_string(uint8_t x, uint8_t y, const char *str) {
        while (*str) {
            oled_draw_char(x, y, *str++);
            x += 6; // Przesunięcie o szerokość znaku (5 pikseli + 1 odstęp)
            if (x >= 128) {
                x = 0;
                y++;
            }
        }
    }

    int main() {
        stdio_init_all();

        // Inicjalizacja I2C
        i2c_init(I2C_PORT, 400 * 1000); // 400 kHz
        gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
        gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
        gpio_pull_up(PIN_SDA);
        gpio_pull_up(PIN_SCL);

        // Inicjalizacja OLED
        oled_init();

        // Pętla główna: wyświetlanie testowego tekstu
        while (true) {
            oled_clear();
            oled_draw_string(0, 0, "test");
            sleep_ms(1000);
        }

        return 0;
    }
#endif

#ifndef OTHER
    #define PIN_CLOCK_power 11

    void clock_init()
    {
        gpio_init(PIN_CLOCK_power);
        gpio_set_dir(PIN_CLOCK_power, GPIO_OUT);
        gpio_put(PIN_CLOCK_power, 1); // Włączamy zasilanie na pinie (3.3V)
    }

    void drawTest(ssd1306_t *pOled){
        ssd1306_draw_string(
                pOled,
                0,
                2,
                2,
                "Testing");

        ssd1306_draw_line(
                pOled,
                2, 25,
                80, 25);
    }

    int main()
    {
        stdio_init_all();
        pico_led_init();
        button_init();
        clock_init();

        //Setup the OLED Screen
        i2c_init(i2c0, 400000);
        
        gpio_pull_up(12);
        gpio_pull_up(13);
        gpio_set_function(12, GPIO_FUNC_I2C);
        gpio_set_function(13, GPIO_FUNC_I2C);
        ssd1306_t oled;
        oled.external_vcc=false;
        bool res = ssd1306_init(
                &oled,
                128,
                64,
                0x3c,
                i2c0);

        //If setup OK then write the test text on OLED
        if (res){
            ssd1306_clear(&oled);
            drawTest(&oled);
            ssd1306_show(&oled);
            // blink_long(3);
            // sleep_ms(5000);
            // blink_quick(3);
        } else {
            printf("Oled Init failed\n");
            // blink_long(6);
            // blink_quick(6);
        }

        while (true)
        {
            if (button_clicked())
            {
                blink_quick(1);
            }
            // else
            // {
            //     ssd1306_clear(&oled);
            //     drawTest(&oled);
            //     ssd1306_show(&oled);
            //     sleep_ms(5000);
            // }
        }

        return 0;
    }
#endif
