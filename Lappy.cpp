#include <stdio.h>
#include "ILI9341_Driver/ILI9341.hpp"
#include "SPI/SPI.h"
#include "pico/stdlib.h"

#define PIN_SCK  2
#define PIN_MOSI 3
#define PIN_CS   1
#define PIN_DC   20

ILI9341 lcd(20);

int main() {
  stdio_init_all();
  sleep_ms(1000);  // Wait for USB serial to be ready

  uint baud = init(80 * 1000 * 1000, PIN_SCK, PIN_MOSI, PIN_CS,
                   true);  // Initialize SPI at 100 MHz
  printf("SPI initialized at baud rate: %.1f MHz\n",
         (float)(baud / (1000 * 1000)));
  lcd.init();

  sleep_ms(1000);  // Wait for display to stabilize
  printf("Entering main loop...\n");

  while (true) {
    lcd.fillScreen(0xF800);  // Fill screen with red
    sleep_ms(1000);
    lcd.fillScreen(0x07E0);  // Fill screen with green
    sleep_ms(1000);
    lcd.fillScreen(0x001F);  // Fill screen with blue
    sleep_ms(1000);
    tight_loop_contents();
  }
}