#ifndef ILI9341_MACROS_H
#define ILI9341_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Level 1 Commands
 *---------------------------------------------------------------------------*/

#define ILI9341_CMD_NOP                        0x00
#define ILI9341_CMD_SOFTWARE_RESET             0x01

#define ILI9341_CMD_READ_ID                    0x04
#define ILI9341_CMD_READ_DISPLAY_STATUS        0x09
#define ILI9341_CMD_READ_POWER_MODE            0x0A
#define ILI9341_CMD_READ_MADCTL                0x0B
#define ILI9341_CMD_READ_PIXEL_FORMAT          0x0C
#define ILI9341_CMD_READ_IMAGE_FORMAT          0x0D
#define ILI9341_CMD_READ_SIGNAL_MODE           0x0E
#define ILI9341_CMD_READ_SELF_DIAGNOSTIC       0x0F

#define ILI9341_CMD_SLEEP_IN                   0x10
#define ILI9341_CMD_SLEEP_OUT                  0x11
#define ILI9341_CMD_PARTIAL_MODE_ON            0x12
#define ILI9341_CMD_NORMAL_DISPLAY_MODE_ON     0x13

#define ILI9341_CMD_DISPLAY_INVERSION_OFF      0x20
#define ILI9341_CMD_DISPLAY_INVERSION_ON       0x21
#define ILI9341_CMD_GAMMA_SET                  0x26

#define ILI9341_CMD_DISPLAY_OFF                0x28
#define ILI9341_CMD_DISPLAY_ON                 0x29

#define ILI9341_CMD_COLUMN_ADDRESS_SET         0x2A
#define ILI9341_CMD_PAGE_ADDRESS_SET           0x2B
#define ILI9341_CMD_MEMORY_WRITE               0x2C
#define ILI9341_CMD_COLOR_SET                  0x2D
#define ILI9341_CMD_MEMORY_READ                0x2E

#define ILI9341_CMD_PARTIAL_AREA               0x30
#define ILI9341_CMD_VERTICAL_SCROLLING_DEF     0x33

#define ILI9341_CMD_TEARING_EFFECT_OFF         0x34
#define ILI9341_CMD_TEARING_EFFECT_ON          0x35

#define ILI9341_CMD_MEMORY_ACCESS_CONTROL      0x36
#define ILI9341_CMD_VERTICAL_SCROLL_START_ADDR 0x37

#define ILI9341_CMD_IDLE_MODE_OFF              0x38
#define ILI9341_CMD_IDLE_MODE_ON               0x39

#define ILI9341_CMD_PIXEL_FORMAT_SET           0x3A

#define ILI9341_CMD_MEMORY_WRITE_CONTINUE      0x3C
#define ILI9341_CMD_MEMORY_READ_CONTINUE       0x3E

#define ILI9341_CMD_SET_TEAR_SCANLINE          0x44
#define ILI9341_CMD_GET_SCANLINE               0x45

#define ILI9341_CMD_WRITE_DISPLAY_BRIGHTNESS   0x51
#define ILI9341_CMD_READ_DISPLAY_BRIGHTNESS    0x52

#define ILI9341_CMD_WRITE_CTRL_DISPLAY         0x53
#define ILI9341_CMD_READ_CTRL_DISPLAY          0x54

#define ILI9341_CMD_WRITE_CABC                 0x55
#define ILI9341_CMD_READ_CABC                  0x56

#define ILI9341_CMD_WRITE_CABC_MIN_BRIGHTNESS  0x5E
#define ILI9341_CMD_READ_CABC_MIN_BRIGHTNESS   0x5F

#define ILI9341_CMD_READ_ID1                   0xDA
#define ILI9341_CMD_READ_ID2                   0xDB
#define ILI9341_CMD_READ_ID3                   0xDC

/*----------------------------------------------------------------------------
 * Level 2 Commands
 *---------------------------------------------------------------------------*/

#define ILI9341_CMD_RGB_INTERFACE_CONTROL     0xB0
#define ILI9341_CMD_FRAME_RATE_NORMAL         0xB1
#define ILI9341_CMD_FRAME_RATE_IDLE           0xB2
#define ILI9341_CMD_FRAME_RATE_PARTIAL        0xB3
#define ILI9341_CMD_DISPLAY_INVERSION_CONTROL 0xB4
#define ILI9341_CMD_BLANKING_PORCH_CONTROL    0xB5
#define ILI9341_CMD_DISPLAY_FUNCTION_CONTROL  0xB6
#define ILI9341_CMD_ENTRY_MODE_SET            0xB7

#define ILI9341_CMD_BACKLIGHT_CONTROL_1       0xB8
#define ILI9341_CMD_BACKLIGHT_CONTROL_2       0xB9
#define ILI9341_CMD_BACKLIGHT_CONTROL_3       0xBA
#define ILI9341_CMD_BACKLIGHT_CONTROL_4       0xBB
#define ILI9341_CMD_BACKLIGHT_CONTROL_5       0xBC
#define ILI9341_CMD_BACKLIGHT_CONTROL_7       0xBE
#define ILI9341_CMD_BACKLIGHT_CONTROL_8       0xBF

#define ILI9341_CMD_POWER_CONTROL_1           0xC0
#define ILI9341_CMD_POWER_CONTROL_2           0xC1

#define ILI9341_CMD_VCOM_CONTROL_1            0xC5
#define ILI9341_CMD_VCOM_CONTROL_2            0xC7

#define ILI9341_CMD_NV_MEMORY_WRITE           0xD0
#define ILI9341_CMD_NV_MEMORY_PROTECTION_KEY  0xD1
#define ILI9341_CMD_NV_MEMORY_STATUS_READ     0xD2
#define ILI9341_CMD_READ_ID4                  0xD3

#define ILI9341_CMD_POSITIVE_GAMMA_CORRECTION 0xE0
#define ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION 0xE1
#define ILI9341_CMD_DIGITAL_GAMMA_CONTROL_1   0xE2
#define ILI9341_CMD_DIGITAL_GAMMA_CONTROL_2   0xE3

#define ILI9341_CMD_INTERFACE_CONTROL         0xF6

/*----------------------------------------------------------------------------
 * Extended Register Commands
 *---------------------------------------------------------------------------*/

#define ILI9341_CMD_POWER_CONTROL_A           0xCB
#define ILI9341_CMD_POWER_CONTROL_B           0xCF

#define ILI9341_CMD_DRIVER_TIMING_CONTROL_A   0xE8
#define ILI9341_CMD_DRIVER_TIMING_CONTROL_A2  0xE9
#define ILI9341_CMD_DRIVER_TIMING_CONTROL_B   0xEA

#define ILI9341_CMD_POWER_ON_SEQUENCE_CONTROL 0xED

#define ILI9341_CMD_ENABLE_3G                 0xF2
#define ILI9341_CMD_PUMP_RATIO_CONTROL        0xF7

/*----------------------------------------------------------------------------
 * MADCTL (0x36) bits
 *---------------------------------------------------------------------------*/

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

/* Common orientations */
#define ILI9341_MADCTL_PORTRAIT     (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)
#define ILI9341_MADCTL_LANDSCAPE    (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)
#define ILI9341_MADCTL_PORTRAIT_INV (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR)
#define ILI9341_MADCTL_LANDSCAPE_INV                           \
  (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | \
   ILI9341_MADCTL_BGR)

/*----------------------------------------------------------------------------
 * COLMOD (0x3A) values
 *---------------------------------------------------------------------------*/

#define ILI9341_COLMOD_12BIT 0x03
#define ILI9341_COLMOD_16BIT 0x05
#define ILI9341_COLMOD_18BIT 0x06
#define ILI9341_COLMOD_16M   0x07

/*----------------------------------------------------------------------------
 * Display geometry
 *---------------------------------------------------------------------------*/

#define ILI9341_WIDTH  240U
#define ILI9341_HEIGHT 320U

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_MACROS_H */