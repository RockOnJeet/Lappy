/*
 * Private GPIO simulation state.
 *
 * Pins are created lazily and stored in a dynamically allocated registry.
 * Each entry is an independent state machine. Transitions are synchronous;
 * this does not create a thread or model a physical CPU core.
 */
#include "gpio_state.h"

#include <stdlib.h>

typedef struct gpio_sim_pin_entry {
    gpio_sim_pin_t pin;
    gpio_sim_pin_state_t state;
    gpio_sim_owner_t owner;
    gpio_sim_role_t role;
    gpio_sim_direction_t direction;
    bool output_latch;
    bool pull_up;
    bool pull_down;
    gpio_sim_external_drive_t external_drive;
    struct gpio_sim_pin_entry *next;
} gpio_sim_pin_entry_t;

static gpio_sim_pin_entry_t *g_pins;

static gpio_sim_pin_entry_t *find_pin(gpio_sim_pin_t pin)
{
    gpio_sim_pin_entry_t *entry;

    for (entry = g_pins; entry != NULL; entry = entry->next) {
        if (entry->pin == pin) {
            return entry;
        }
    }
    return NULL;
}

static gpio_sim_pin_entry_t *get_or_create_pin(gpio_sim_pin_t pin)
{
    gpio_sim_pin_entry_t *entry = find_pin(pin);

    if (entry != NULL) {
        return entry;
    }

    entry = (gpio_sim_pin_entry_t *)calloc(1, sizeof(*entry));
    if (entry == NULL) {
        return NULL;
    }

    entry->pin = pin;
    entry->state = GPIO_SIM_PIN_UNASSIGNED;
    entry->direction = GPIO_SIM_DIRECTION_INPUT;
    entry->external_drive = GPIO_SIM_EXTERNAL_FLOATING;
    entry->next = g_pins;
    g_pins = entry;
    return entry;
}

static bool owner_is_valid(gpio_sim_owner_t owner)
{
    return owner > GPIO_SIM_OWNER_NONE && owner <= GPIO_SIM_OWNER_OTHER;
}

static gpio_sim_status_t get_owned_pin(
    gpio_sim_pin_t pin,
    gpio_sim_owner_t owner,
    gpio_sim_pin_entry_t **entry_out)
{
    gpio_sim_pin_entry_t *entry;

    if (!owner_is_valid(owner) || entry_out == NULL) {
        return GPIO_SIM_INVALID_ARGUMENT;
    }

    entry = find_pin(pin);
    if (entry == NULL) {
        return GPIO_SIM_NOT_FOUND;
    }
    if (entry->state != GPIO_SIM_PIN_ASSIGNED || entry->owner != owner) {
        return GPIO_SIM_NOT_OWNER;
    }

    *entry_out = entry;
    return GPIO_SIM_OK;
}

static bool resolve_level(const gpio_sim_pin_entry_t *entry)
{
    if (entry->direction == GPIO_SIM_DIRECTION_OUTPUT) {
        return entry->output_latch;
    }
    if (entry->external_drive == GPIO_SIM_EXTERNAL_HIGH) {
        return true;
    }
    if (entry->external_drive == GPIO_SIM_EXTERNAL_LOW) {
        return false;
    }
    if (entry->pull_up && !entry->pull_down) {
        return true;
    }

    /* Pull-down and floating inputs resolve LOW in this logical model. */
    return false;
}

gpio_sim_status_t gpio_sim_claim(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner, gpio_sim_role_t role)
{
    gpio_sim_pin_entry_t *entry;

    if (!owner_is_valid(owner) ||
        role < GPIO_SIM_ROLE_GENERAL || role > GPIO_SIM_ROLE_UART_RX) {
        return GPIO_SIM_INVALID_ARGUMENT;
    }

    entry = get_or_create_pin(pin);
    if (entry == NULL) {
        return GPIO_SIM_OUT_OF_MEMORY;
    }

    if (entry->state == GPIO_SIM_PIN_ASSIGNED) {
        if (entry->owner == owner && entry->role == role) {
            return GPIO_SIM_OK;
        }
        return GPIO_SIM_OWNERSHIP_CONFLICT;
    }

    entry->state = GPIO_SIM_PIN_ASSIGNED;
    entry->owner = owner;
    entry->role = role;
    entry->direction = GPIO_SIM_DIRECTION_INPUT;
    entry->output_latch = false;
    entry->pull_up = false;
    entry->pull_down = false;
    entry->external_drive = GPIO_SIM_EXTERNAL_FLOATING;
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_release(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner)
{
    gpio_sim_pin_entry_t *entry;
    gpio_sim_status_t status = get_owned_pin(pin, owner, &entry);

    if (status != GPIO_SIM_OK) {
        return status;
    }

    entry->state = GPIO_SIM_PIN_UNASSIGNED;
    entry->owner = GPIO_SIM_OWNER_NONE;
    entry->role = GPIO_SIM_ROLE_NONE;
    entry->direction = GPIO_SIM_DIRECTION_INPUT;
    entry->output_latch = false;
    entry->pull_up = false;
    entry->pull_down = false;
    entry->external_drive = GPIO_SIM_EXTERNAL_FLOATING;
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_set_direction(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner,
    gpio_sim_direction_t direction)
{
    gpio_sim_pin_entry_t *entry;
    gpio_sim_status_t status;

    if (direction != GPIO_SIM_DIRECTION_INPUT &&
        direction != GPIO_SIM_DIRECTION_OUTPUT) {
        return GPIO_SIM_INVALID_ARGUMENT;
    }

    status = get_owned_pin(pin, owner, &entry);
    if (status != GPIO_SIM_OK) {
        return status;
    }
    entry->direction = direction;
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_write(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner, bool level)
{
    gpio_sim_pin_entry_t *entry;
    gpio_sim_status_t status = get_owned_pin(pin, owner, &entry);

    if (status != GPIO_SIM_OK) {
        return status;
    }

    /* Retain the output latch even while the pin is configured as input. */
    entry->output_latch = level;
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_read(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner, bool *level)
{
    gpio_sim_pin_entry_t *entry;
    gpio_sim_status_t status;

    if (level == NULL) {
        return GPIO_SIM_INVALID_ARGUMENT;
    }
    status = get_owned_pin(pin, owner, &entry);
    if (status != GPIO_SIM_OK) {
        return status;
    }

    *level = resolve_level(entry);
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_set_pulls(
    gpio_sim_pin_t pin, gpio_sim_owner_t owner,
    bool pull_up, bool pull_down)
{
    gpio_sim_pin_entry_t *entry;
    gpio_sim_status_t status;

    if (pull_up && pull_down) {
        return GPIO_SIM_INVALID_ARGUMENT;
    }
    status = get_owned_pin(pin, owner, &entry);
    if (status != GPIO_SIM_OK) {
        return status;
    }

    entry->pull_up = pull_up;
    entry->pull_down = pull_down;
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_drive_input(gpio_sim_pin_t pin, bool level)
{
    gpio_sim_pin_entry_t *entry = get_or_create_pin(pin);

    if (entry == NULL) {
        return GPIO_SIM_OUT_OF_MEMORY;
    }
    if (entry->state == GPIO_SIM_PIN_ASSIGNED &&
        entry->direction == GPIO_SIM_DIRECTION_OUTPUT) {
        return GPIO_SIM_OWNERSHIP_CONFLICT;
    }

    entry->external_drive = level
        ? GPIO_SIM_EXTERNAL_HIGH
        : GPIO_SIM_EXTERNAL_LOW;
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_release_input(gpio_sim_pin_t pin)
{
    gpio_sim_pin_entry_t *entry = find_pin(pin);

    if (entry == NULL) {
        return GPIO_SIM_NOT_FOUND;
    }
    entry->external_drive = GPIO_SIM_EXTERNAL_FLOATING;
    return GPIO_SIM_OK;
}

gpio_sim_status_t gpio_sim_get_snapshot(
    gpio_sim_pin_t pin, gpio_sim_pin_snapshot_t *snapshot)
{
    gpio_sim_pin_entry_t *entry = find_pin(pin);

    if (snapshot == NULL) {
        return GPIO_SIM_INVALID_ARGUMENT;
    }
    if (entry == NULL) {
        return GPIO_SIM_NOT_FOUND;
    }

    snapshot->pin = entry->pin;
    snapshot->state = entry->state;
    snapshot->owner = entry->owner;
    snapshot->role = entry->role;
    snapshot->direction = entry->direction;
    snapshot->output_latch = entry->output_latch;
    snapshot->pull_up = entry->pull_up;
    snapshot->pull_down = entry->pull_down;
    snapshot->external_drive = entry->external_drive;
    snapshot->resolved_level = resolve_level(entry);
    snapshot->floating =
        entry->direction == GPIO_SIM_DIRECTION_INPUT &&
        entry->external_drive == GPIO_SIM_EXTERNAL_FLOATING &&
        !entry->pull_up && !entry->pull_down;
    return GPIO_SIM_OK;
}
