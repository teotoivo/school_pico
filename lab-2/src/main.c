#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "stdio.h"
#include "utils.h"

#define LED_D1 22
#define LED_D2 21
#define LED_D3 20

#define ROT_SW 12

#define ROT_A 10
#define ROT_B 11

#define DEFAULT_BRIGHTNESS 500

#define SCROLL_SPEED 75

static bool led_on = false;
volatile static int brightness = DEFAULT_BRIGHTNESS;

static queue_t events;

void gpio_callback(uint gpio, uint32_t event_mask) {
  // events is checked with the & operator because it is a bit mask this will
  // check if one of the events is GPIO_IRQ_EDGE_RISE
  if (gpio == ROT_A && (event_mask & GPIO_IRQ_EDGE_RISE)) {

    if (gpio_get(ROT_B) == 0) {
      // clockwise turn
      if (led_on) {
        int rot_direction = 1;
        queue_try_add(&events, &rot_direction);
      }
    } else {
      // counter clockwise turn
      if (led_on) {
        int rot_direction = -1;
        queue_try_add(&events, &rot_direction);
      }
    }
  }
}

int main() {
  stdio_init_all();

  bool sw_held_down = false;

  init_button_pin(ROT_SW);
  init_rot_pin(ROT_A);
  init_rot_pin(ROT_B);

  setup_pwm(LED_D1);
  setup_pwm(LED_D2);
  setup_pwm(LED_D3);

  queue_init(&events, sizeof(int), 10);

  // sets the callback for all enabled interrupts
  gpio_set_irq_enabled_with_callback(ROT_A, GPIO_IRQ_EDGE_RISE, true,
                                     &gpio_callback);
  // Loop forever
  while (true) {
    int rot_direction;
    while (queue_try_remove(&events, &rot_direction)) {
      brightness += rot_direction * SCROLL_SPEED;
    }

    if (sw_held_down && !button_pressed(ROT_SW)) {
      // if button is released reset sw_held_down
      sw_held_down = false;
    } else if (!sw_held_down && button_pressed(ROT_SW)) {
      // if button is pressed for the fisrt time toggle led

      if (led_on && brightness == 0) {
        // if toggled on and brightnes is zero reset brightness to 500
        brightness = DEFAULT_BRIGHTNESS;
      } else {
        led_on = !led_on;
      }
      sw_held_down = true;
      sleep_ms(250); // wait to avoid switch bounce
    }

    if (led_on) {
      if (brightness > 1000)
        brightness = 1000;
      if (brightness < 0)
        brightness = 0;

      set_brightness(LED_D1, brightness);
      set_brightness(LED_D2, brightness);
      set_brightness(LED_D3, brightness);
    } else {
      set_brightness(LED_D1, 0);
      set_brightness(LED_D2, 0);
      set_brightness(LED_D3, 0);
    }
  }
}
