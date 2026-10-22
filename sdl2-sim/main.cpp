#include <SDL.h>

#include <cstdio>

static constexpr int DISPLAY_WIDTH = 240;
static constexpr int DISPLAY_HEIGHT = 320;

static uint32_t rgb888(float r, float g, float b) {
  uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
  uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
  uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
  return ((uint32_t)r8 << 16) | ((uint32_t)g8 << 8) | (uint32_t)b8;
}

static uint16_t rgb565(float r, float g, float b) {
  uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
  uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
  uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
  return ((r8 & 0xF8) << 8) | ((g8 & 0xFC) << 3) | (b8 >> 3);
}

enum class PixelFormat : uint8_t {
  RGB12   = 0x03,  // 12-bit RGB
  RGB565  = 0x05,  // 16-bit RGB
  RGB666  = 0x06,  // 18-bit RGB
  RGB888  = 0x07,  // 24-bit RGB
};

static const char* pixel_format_name(PixelFormat fmt) {
  switch (fmt) {
    case PixelFormat::RGB12:   return "RGB12";
    case PixelFormat::RGB565:  return "RGB565";
    case PixelFormat::RGB666:  return "RGB666";
    case PixelFormat::RGB888:  return "RGB888";
  }
  return "UNKNOWN";
}

int main(int, char**) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_version version;
  SDL_GetVersion(&version);
  std::printf("SDL %d.%d.%d\n", version.major, version.minor, version.patch);

  SDL_Window* window =
      SDL_CreateWindow("Lappy SDL2 Sim", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                       SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  }
  if (renderer == nullptr) {
    std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_RendererInfo info;
  if (SDL_GetRendererInfo(renderer, &info) == 0) {
    std::printf("Video driver: %s\n", SDL_GetCurrentVideoDriver());
    std::printf("Renderer: %s\n", info.name);
  }

  // Gram buffer: uint32_t supports all pixel formats (12/16/18/24-bit)
  // Each pixel takes 4 bytes; unused bits are padding
  uint32_t* gram = static_cast<uint32_t*>(SDL_malloc(
      static_cast<size_t>(DISPLAY_WIDTH) * DISPLAY_HEIGHT * 4));

  PixelFormat pixel_format = PixelFormat::RGB565;  // Current format

  // Color definitions (full RGB, not premultiplied)
  struct Color { uint8_t r, g, b; };
  Color colors[7] = {
    {255, 255, 255}, // white
    {255,   0,   0}, // red
    {  0, 255,   0}, // green
    {  0,   0, 255}, // blue
    {  0, 255, 255}, // cyan
    {255,   0, 255}, // magenta
    {128,  128,  128} // gray
  };

  int rows_per_color = DISPLAY_HEIGHT / 7;  // 320/7 = 45 rows per color
  int remainder = DISPLAY_HEIGHT % 7;       // 320%7 = 5 extra rows

  bool running = true;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN &&
                 event.key.keysym.sym == SDLK_ESCAPE) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        // Cycle pixel format on any key press
        switch (pixel_format) {
          case PixelFormat::RGB12:   pixel_format = PixelFormat::RGB565; break;
          case PixelFormat::RGB565:  pixel_format = PixelFormat::RGB666; break;
          case PixelFormat::RGB666:  pixel_format = PixelFormat::RGB888; break;
          case PixelFormat::RGB888:  pixel_format = PixelFormat::RGB12; break;
        }
        std::printf("Pixel format: %s\n", pixel_format_name(pixel_format));
      }
    }

    // Fill GRAM with color strips: rows_per_color rows per color, brightness decreasing left→right
    for (int c = 0; c < 7; ++c) {
      for (int y = 0; y < rows_per_color; ++y) {
        int yy = c * rows_per_color + y;
        if (yy >= DISPLAY_HEIGHT) break;
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
          float brightness = 1.0f - (x / static_cast<float>(DISPLAY_WIDTH));
          // Apply brightness to color components (0.0-1.0 range)
          float r = colors[c].r / 255.0f * brightness;
          float g = colors[c].g / 255.0f * brightness;
          float b = colors[c].b / 255.0f * brightness;

          // Store to gram based on pixel format
          uint32_t color_word = 0;
          switch (pixel_format) {
            case PixelFormat::RGB12:
              // 12-bit: 4 bits per channel (RRRR GGGG BBBB) shifted
              color_word = ((uint32_t)(static_cast<uint8_t>(r * 15.0f)) << 8) |
                           ((uint32_t)(static_cast<uint8_t>(g * 15.0f)) << 4) |
                            (uint32_t)(static_cast<uint8_t>(b * 15.0f));
              break;
            case PixelFormat::RGB565:
              color_word = rgb565(r, g, b);
              break;
            case PixelFormat::RGB666:
              color_word = ((uint32_t)(static_cast<uint8_t>(r * 63.0f)) << 16) |
                           ((uint32_t)(static_cast<uint8_t>(g * 63.0f)) << 8) |
                            (uint32_t)(static_cast<uint8_t>(b * 63.0f));
              break;
            case PixelFormat::RGB888:
              color_word = rgb888(r, g, b);
              break;
          }
          gram[yy * DISPLAY_WIDTH + x] = color_word;
        }
      }
    }
    // Fill any leftover rows (remainder) with black
    for (int yy = 7 * rows_per_color; yy < DISPLAY_HEIGHT; ++yy) {
      for (int xx = 0; xx < DISPLAY_WIDTH; ++xx) {
        gram[yy * DISPLAY_WIDTH + xx] = 0;
      }
    }

    // Software render: draw each pixel from the GRAM (pixel format-dependent)
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
      for (int x = 0; x < DISPLAY_WIDTH; ++x) {
        uint32_t color = gram[y * DISPLAY_WIDTH + x];
        uint8_t r, g, b;

        // Expand from gram based on pixel format
        switch (pixel_format) {
          case PixelFormat::RGB12:
            r = static_cast<uint8_t>((color >> 8) & 0xF) * 17;   // 4-bit → 8-bit ×17
            g = static_cast<uint8_t>((color >> 4) & 0xF) * 17;   // 4-bit → 8-bit ×17
            b = static_cast<uint8_t>( color        & 0xF) * 17;   // 4-bit → 8-bit ×17
            break;
          case PixelFormat::RGB565:
            r = static_cast<uint8_t>((color >> 11) & 0x1F) * 8;
            g = static_cast<uint8_t>((color >> 5) & 0x3F) * 4;
            b = static_cast<uint8_t>( color        & 0x1F) * 8;
            break;
          case PixelFormat::RGB666:
            r = static_cast<uint8_t>((color >> 16) & 0x3F) * 4;   // 6-bit → 8-bit ×4
            g = static_cast<uint8_t>((color >> 8)  & 0x3F) * 4;
            b = static_cast<uint8_t>( color        & 0x3F) * 4;
            break;
          case PixelFormat::RGB888:
            r = static_cast<uint8_t>((color >> 16) & 0xFF);
            g = static_cast<uint8_t>((color >> 8)  & 0xFF);
            b = static_cast<uint8_t>( color        & 0xFF);
            break;
        }
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, x, y);
      }
    }

    SDL_RenderPresent(renderer);
  }

  SDL_free(gram);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}