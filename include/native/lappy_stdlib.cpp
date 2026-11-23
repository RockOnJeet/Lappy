#include "lappy_stdlib.h"
#include <chrono>
#include <thread>

void stdio_init(void) {
  // Nothing to do. The program attaches to terminal output.
}

void delay(uint32_t ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
