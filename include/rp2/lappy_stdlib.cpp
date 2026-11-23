#include "lappy_stdlib.h"
#include "pico/stdlib.h"

void stdio_init(void) {
  stdio_init_all();
}

void delay(uint32_t ms) {
  sleep_ms(ms);
}
