#include "__preprocessor__.h"
#include "pico/stdlib.h"

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

#define NUM_OF_BLINKS   ( 3 )
#define TIME_QUICK      ( 100 )
#define TIME_LONG       ( 250 )

void blink_quick()
{
    pico_set_led(true);
    sleep_ms(TIME_QUICK);
    pico_set_led(false);
    sleep_ms(TIME_QUICK);
}
void blink_long()
{
    pico_set_led(true);
    sleep_ms(TIME_LONG);
    pico_set_led(false);
    sleep_ms(TIME_LONG);
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
bool button_status()
{
    return gpio_get(PIN_BUTTON_output) == 1;
}

#define PIN_BUTTON_output 15

int main()
{
    pico_led_init();
    button_init();

    while (true)
    {
        if (button_status())
        {
            blink_quick();
        }
    }

    return 0;
}
