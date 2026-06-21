
# Lappy - Your personal connected assistant

This project entails an attempt to build a **small-screen assistant OS**, capable of integrating with various APIs and running on low-resource hardware.

  

## Specs

- Runs on RP2x devices

- Controls ILI9341 TFT LCD

- Supports SPI transactions with DMA

- Capable of locally storing and running ~3MB worth of GIFs

- Fast enough to play DOOM, probably (should test that next)

- Doesn't support wireless currentl, hence communicates via USB

- Can show GIFs, system status, custom frontends if needed

  

## Current Implementations

- **V0 (current):** Full display pipeline done; shows looping RGB colors for now.

- **V1:** GIF processor done. GIFs found on-device will play one after another in loop.

- **V2:** USB support done. A custom Python node does preprocessing of GIFs to auto-send to the device for playback.

- **V3:** Custom frontend support - Use web designing tools to render the pages of your choice, define static v/s dynamic parts for display rendering through Python.

- **V4:** (To be continued)

  

## How to install/run this

### Setup VSCode

Install and setup extension: [Raspberry Pi Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

  

### Flash the code

Assuming `git` installed on host, run:

```

git clone https://github.com/RockOnJeet/Lappy.git

cd Lappy

pwd

```

*Note the location above* and open the directory on VSCode.

Use **"compile"** button to verify if the code compiles for your device.

Use **"upload"** when RP2x device is in BOOTSEL mode to flash.

  

### Enjoy!