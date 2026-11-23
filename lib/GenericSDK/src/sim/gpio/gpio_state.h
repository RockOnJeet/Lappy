/*
 * Private GPIO simulation-state interface.
 * Application code must use the native SDK API instead.
 */
#ifndef GENERIC_SDK_GPIO_STATE_H
#define GENERIC_SDK_GPIO_STATE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t gpio_sim_pin_t;

typedef enum {
    GPIO_SIM_OWNER_NONE = 0,
    GPIO_SIM_OWNER_SIO,
    GPIO_SIM_OWNER_SPI,
    GPIO_SIM_OWNER_I2C,
    GPIO_SIM_OWNER_PWM,
    GPIO_SIM_OWNER_UART,
    GPIO_SIM_OWNER_OTHER
} gpio_sim_owner_t;

typedef enum {
    GPIO_SIM_ROLE_NONE = 0,
    GPIO_SIM_ROLE_GENERAL,
    GPIO_SIM_ROLE_SPI_SCK,
    GPIO_SIM_ROLE_SPI_MOSI,
    GPIO_SIM_ROLE_SPI_MISO,
    GPIO_SIM_ROLE_I2C_SDA,
    GPIO_SIM_ROLE_I2C_SCL,
    GPIO_SIM_ROLE_PWM_OUT,
    GPIO_SIM_ROLE_UART_TX,
    GPIO_SIM_ROLE_UART_RX
} gpio_sim_role_t;

typedef enum {
    GPIO_SIM_DIRECTION_INPUT = 0,
    GPIO_SIM_DIRECTION_OUTPUT
} gpio_sim_direction_t;

typedef enum {
    GPIO_SIM_PIN_UNASSIGNED = 0,
    GPIO_SIM_PIN_ASSIGNED
} gpio_sim_pin_state_t;

typedef enum {
    GPIO_SIM_EXTERNAL_FLOATING = 0,
    GPIO_SIM_EXTERNAL_LOW,
    GPIO_SIM_EXTERNAL_HIGH
} gpio_sim_external_drive_t;

typedef enum {
    GPIO_SIM_OK = 0,
    GPIO_SIM_INVALID_ARGUMENT,
    GPIO_SIM_OWNERSHIP_CONFLICT,
    GPIO_SIM_NOT_OWNER,
    GPIO_SIM_NOT_FOUND,
    GPIO_SIM_OUT_OF_MEMORY
} gpio_sim_status_t;

typedef struct {
    gpio_sim_pin_t pin;
    gpio_sim_pin_state_t state;
    gpio_sim_owner_t owner;
    gpio_sim_role_t role;
    gpio_sim_direction_t direction;
    bool output_latch;
    bool pull_up;
    bool pull_down;
    gpio_sim_external_drive_t external_drive;
    bool resolved_level;
    bool floating;
} gpio_sim_pin_snapshot_t;

/* Claim is idempotent only for the same owner and role. */
gpio_sim_status_t gpio_sim_claim(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner, gpio_sim_role_t role);
gpio_sim_status_t gpio_sim_release(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner);

gpio_sim_status_t gpio_sim_set_direction(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner,
    gpio_sim_direction_t direction);
gpio_sim_status_t gpio_sim_write(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner, bool level);
gpio_sim_status_t gpio_sim_read(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner, bool *level);
gpio_sim_status_t gpio_sim_set_pulls(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner,
    bool pull_up, bool pull_down);

/* Simulated external devices use these to drive/release an input pin. */
gpio_sim_status_t gpio_sim_drive_input(gpio_sim_pin_t pin, bool level);
gpio_sim_status_t gpio_sim_release_input(gpio_sim_pin_t pin);

/* Internal diagnostic/test access; not part of the public SDK. */
gpio_sim_status_t gpio_sim_get_snapshot(
    gpio_sim_pin_t pin, gpio_sim_pin_snapshot_t *snapshot);

#ifdef __cplusplus
}
#endif

#endif
