#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include <hardware/gpio.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPTO_FORK 28 // Input with pullup

#define STEPPER_CONTROLLER_0 2
#define STEPPER_CONTROLLER_1 3
#define STEPPER_CONTROLLER_2 6
#define STEPPER_CONTROLLER_3 13

// Global variables: default steps per revolution and current stepping index.
uint steps_per_rev = 4096;
uint current_step = 0;
bool is_calibrated = false; // Indicates if calibration has been performed

static queue_t event_queue;

// Function Prototypes
void init_peripherals(void);
void init_stepper_pin(uint pin);
char *readline(void);
int startsWith(const char *str, const char *prefix);
void process_command(const char *line);
void run_stepper_steps(int steps_to_run);
void calibrate(void);
void gpio_callback(uint gpio, uint32_t event_mask);

// Half-step sequence for the stepper motor
const uint8_t steps[8][4] = {
    {1, 0, 0, 0}, // Step 1
    {1, 1, 0, 0}, // Step 2
    {0, 1, 0, 0}, // Step 3
    {0, 1, 1, 0}, // Step 4
    {0, 0, 1, 0}, // Step 5
    {0, 0, 1, 1}, // Step 6
    {0, 0, 0, 1}, // Step 7
    {1, 0, 0, 1}  // Step 8
};

int main() {
  // Initialize serial communication
  stdio_init_all();
  sleep_ms(3000); // Give time for the serial console to connect

  // Initialize all necessary peripherals
  init_peripherals();

  queue_init(&event_queue, sizeof(int), 20);

  gpio_set_irq_enabled_with_callback(OPTO_FORK, GPIO_IRQ_EDGE_FALL, true,
                                     &gpio_callback);

  // Command loop
  while (true) {
    printf(" > ");
    char *line = readline();
    process_command(line);
    free(line);
  }
}

void calibrate(void) {
  printf("Calibrating...\n");
  // clear event queue
  int event;
  while (queue_try_remove(&event_queue, &event))
    ;

  while (true) {
    if (queue_try_remove(&event_queue, &event)) {
      break;
    }
    run_stepper_steps(1);
  }

  while (queue_try_remove(&event_queue, &event))
    ;

  uint steps_arr[3];

  for (int i = 0; i < 3; i++) {
    uint steps = 0;
    while (true) {
      if (queue_try_remove(&event_queue, &event)) {
        break;
      }
      run_stepper_steps(1);
      steps++;
    }
    steps_arr[i] = steps;
  }

  steps_per_rev = (steps_arr[0] + steps_arr[1] + steps_arr[2]) / 3;
  is_calibrated = true;
}

// Initialize peripherals: opto fork input and stepper motor controller pins.
void init_peripherals(void) {
  // Configure the opto fork input with pull-up.
  gpio_init(OPTO_FORK);
  gpio_set_dir(OPTO_FORK, GPIO_IN);
  gpio_pull_up(OPTO_FORK);

  // Configure stepper controller pins as outputs.
  init_stepper_pin(STEPPER_CONTROLLER_0);
  init_stepper_pin(STEPPER_CONTROLLER_1);
  init_stepper_pin(STEPPER_CONTROLLER_2);
  init_stepper_pin(STEPPER_CONTROLLER_3);
}

// Initialize a given pin as a digital output.
void init_stepper_pin(uint pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
}

// Process commands received from the serial console.
void process_command(const char *line) {
  if (startsWith(line, "run")) {
    // Skip the "run" command and any following whitespace.
    const char *arg = line + 3;
    while (*arg == ' ') {
      arg++;
    }
    if (*arg != '\0') {
      char *endptr;
      long value = strtol(arg, &endptr, 10);
      if (arg == endptr) {
        printf("No valid integer found after 'run'\n");
      } else {
        int cycles = (steps_per_rev / 8) * value;
        run_stepper_steps(cycles);
      }
    } else {
      // If no argument is provided, run one full revolution.
      run_stepper_steps(steps_per_rev);
    }
  } else if (startsWith(line, "calib")) {
    calibrate();
    printf("Calibration done.\n");
    printf("Steps per rev %d\n", steps_per_rev);
    stdio_flush();
  } else if (startsWith(line, "status")) {
    printf("System status:\n");
    printf("  Calibrated: %s\n", is_calibrated ? "Yes" : "No");
    if (is_calibrated)
      printf("  Steps per revolution: %d\n", steps_per_rev);
    else
      printf("  Steps per revolution: not available\n");
  } else {
    printf("Unknown command: %s\n", line);
  }
}

// Execute the step sequence for the specified number of steps.
void run_stepper_steps(int steps_to_run) {
  const int32_t SLEEP_DELAY = 1500;
  if (steps_to_run > 0) {
    for (int i = 0; i < steps_to_run; i++) {
      gpio_put(STEPPER_CONTROLLER_0, steps[current_step][0]);
      gpio_put(STEPPER_CONTROLLER_1, steps[current_step][1]);
      gpio_put(STEPPER_CONTROLLER_2, steps[current_step][2]);
      gpio_put(STEPPER_CONTROLLER_3, steps[current_step][3]);
      current_step = (current_step + 1) % 8;
      sleep_us(SLEEP_DELAY);
    }
  } else if (steps_to_run < 0) {
    for (int i = 0; i < abs(steps_to_run); i++) {
      gpio_put(STEPPER_CONTROLLER_0, steps[current_step][0]);
      gpio_put(STEPPER_CONTROLLER_1, steps[current_step][1]);
      gpio_put(STEPPER_CONTROLLER_2, steps[current_step][2]);
      gpio_put(STEPPER_CONTROLLER_3, steps[current_step][3]);
      current_step = (current_step - 1 + 8) % 8;
      sleep_us(SLEEP_DELAY);
    }
  }
}

// Reads a line of input from the serial console.
char *readline() {
  uint size = 64;
  char *line = malloc(size);
  uint pos = 0;
  int c;

  while ((c = getchar()) != '\n' && c != '\r' && c != EOF) {
    printf("%c", c); // Echo the character for feedback.
    line[pos++] = (char)c;
    if (pos >= size) {
      size *= 2;
      line = realloc(line, size);
    }
  }
  line[pos] = '\0';
  printf("\n");
  return line;
}

// Checks if the string 'str' starts with 'prefix'.
int startsWith(const char *str, const char *prefix) {
  size_t lenPrefix = strlen(prefix);
  return strncmp(str, prefix, lenPrefix) == 0;
}

void gpio_callback(uint gpio, uint32_t event_mask) {
  if (gpio == OPTO_FORK && (event_mask & GPIO_IRQ_EDGE_FALL)) {
    int event = 1;
    queue_try_add(&event_queue, &event);
  }
}
