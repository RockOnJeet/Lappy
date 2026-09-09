#ifndef ILI9341_HPP
#define ILI9341_HPP

#include <math.h>
#include <cstring>
#include "ILI9341_macros.h"
#include "SPI/SPI.hpp"
#include "pico/stdlib.h"

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
   * @param spi Pointer to the SPI object for communication
   * @param use_dma Optional flag to enable DMA for pixel transfers (default:
   * false)
   * @note If use_dma is true, the SPI object must support DMA transfers.
   */
  void init(SPI* spi, bool use_dma = false);
  /**
   * @brief Rotate the display to a specific orientation
   * @param rotation The rotation value (0-3) corresponding to 0, 90, 180, and
   * 270 degrees.
   * @note Handles overflow and wraps around if the rotation value exceeds 3.
   */
  void setRotation(uint8_t rotation);
  /**
   * @brief Get the current width of the display based on rotation
   * @return The current width of the display in pixels.
   */
  inline uint16_t getWidth() const { return _width; }
  /**
   * @brief Get the current height of the display based on rotation
   * @return The current height of the display in pixels.
   */
  inline uint16_t getHeight() const { return _height; }
  /**
   * @brief Set the address window for subsequent pixel writes
   * @param x0 The starting x coordinate of the window (from top-left corner)
   * @param y0 The starting y coordinate of the window (from top-left corner)
   * @param w The width of the window
   * @param h The height of the window
   * @return true if the address window was set successfully, false otherwise
   */
  bool setAddressWindow(int32_t x0, int32_t y0, int32_t w, int32_t h);
  /**
   * @brief Fill the entire screen with a color
   * @param color The RGB565 color to fill the screen with.
   */
  void fillScreen(uint16_t color);
  /**
   * @brief Clear the entire screen (fill with black)
   */
  void clearScreen() { fillScreen(0); }
  /**
   * @brief Setup the SPI bus for continuous communication with the display
   * @warning This function does not set the address window or send pixel data.
   */
  void beginStream(int32_t x0, int32_t y0, int32_t w, int32_t h);
  /**
   * @brief Get the active buffer for pixel data manipulation
   * @param length Reference variable to store the length of the active buffer
   * @return Pointer to the active buffer
   */
  uint16_t* getActiveBuffer(size_t& length);
  /**
   * @brief Submit the currently active buffer to the display for rendering
   * @param length The length of the active buffer to be submitted.
   * @note Streams the buffer directly to the display; setAddressWindow
   * determines the area to be updated.
   * @warning The function may block until the SPI bus is free, especially if
   * DMA is used for pixel transfers.
   */
  void submitActiveBuffer(size_t length);
  /**
   * @brief Close the SPI bus after communication with the display
   * @warning This function does not send pixel data.
   */
  void endStream();

 private:
  SPI* _spi;  // Pointer to the SPI object for communication with the display
  uint16_t _dcPin;
  uint16_t _rstPin;
  uint16_t _width = ILI9341_WIDTH;    // Current width based on rotation
  uint16_t _height = ILI9341_HEIGHT;  // Current height based on rotation
  bool _use_dma = false;  // Flag to indicate if DMA is used for pixel transfers
  size_t _ext_size = 0;   // Size of the external buffer in pixels

  // Buffer array for state management
  uint16_t _buffer[ILI9341_WIDTH *
                   ILI9341_HEIGHT];   // Internal buffer for pixel data
  uint16_t* _activeBuffer = _buffer;  // Pointer to buffer

  // Write a single command byte to the display
  void writeCmd(uint8_t cmd);
  // Write a single data byte to the display
  void writeData(uint8_t data);
};

#endif  // ILI9341_HPP