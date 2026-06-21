#ifndef ILI9341_HPP
#define ILI9341_HPP

#include <math.h>
#include <pico/stdlib.h>
#include "../SPI/SPI.h"
#include "ILI9341_macros.h"

class ILI9341 {
 public:
  /**
   * @brief Construct a new ILI9341 object
   */
  ILI9341(uint16_t dc, uint16_t rst = -1);

  /**
   * @brief Destroy the ILI9341 object
   */
  ~ILI9341();

  /**
   * @brief Initialize the ILI9341 display
   */
  void init();
  inline void begin() { init(); }
  /**
   * @brief Fill the entire screen with a color
   * @param color The color to fill the screen with
   * @note This function is not optimized for speed. Use with caution.
   */
  void fillScreen(uint16_t color);
  /**
   * @brief Clear the entire screen (fill with black)
   * @note This function is not optimized for speed. Use with caution.
   */
  void clearScreen() { fillScreen(0); }
  /**
   * @brief Set the address window for subsequent pixel writes
   * @param x0 The starting x coordinate of the window
   * @param y0 The starting y coordinate of the window
   * @param w The width of the window
   * @param h The height of the window
   * @return true if the address window was set successfully, false otherwise
   */
  bool setAddressWindow(int16_t x0, int16_t y0, int16_t w, int16_t h);
  /**
   * @brief Draw a single pixel at the specified coordinates with the specified
   * color
   * @param x The x coordinate of the pixel
   * @param y The y coordinate of the pixel
   * @param color The color of the pixel
   * @note Position is relative to the address window set and rotation; Use
   * pushColors() for faster drawing of multiple pixels
   */
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  /**
   * @brief Push a list of colors to the display
   * @param data A pointer to the array of colors to push
   * @param len The number of colors to push
   * @param first Whether this is the first set of colors to push
   */
  void pushColors(uint16_t* data, int16_t len, bool first);
  /**
   * @brief Push a list of pixels to the display
   * @param data A pointer to the array of pixels to push
   * @param len The number of pixels to push
   * @param first Whether this is the first set of pixels to push
   */
  void pushPixels(uint16_t* data, int16_t len, bool first) {
    pushColors(data, len, first);
  }

 private:
  uint16_t _dcPin;
  uint16_t _rstPin;

  void writeCmd(uint8_t cmd);
  void writeData(uint8_t data);
};

#endif  // ILI9341_HPP