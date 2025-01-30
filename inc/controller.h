#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "config.h"
#include <stdint.h>
#include <stdbool.h>

// Phase numbers based on NEMA standard
typedef enum
{
    PHASE_1, // Left turn 1 (main road)
    PHASE_2, // Through 1 (main road)
    PHASE_3, // Left turn 2 (cross road)
    PHASE_4, // Through 2 (cross road)
    PHASE_5, // Left turn 3 (main road)
    PHASE_6, // Through 3 (main road)
    PHASE_7, // Left turn 4 (cross road)
    PHASE_8, // Through 4 (cross road)
    NUM_PHASES
} phase_t;

typedef enum
{
    SIGNAL_RED,
    SIGNAL_YELLOW,
    SIGNAL_GREEN,
    SIGNAL_FLASH_YELLOW,
    SIGNAL_FLASH_RED,
    SIGNAL_OFF
} signal_state_t;

typedef struct
{
    phase_t current_phase;
    signal_state_t main_state;
    signal_state_t side_state;
    uint32_t phase_timer; // Time in current phase
} controller_state_t;

typedef struct controller controller_t;
struct controller
{
    const config_t *config;
    controller_state_t state;
    bool (*init)(controller_t *const controller);
    void (*run)(controller_t *const controller);
};

bool controller_init(controller_t *const controller);

#endif // CONTROLLER_H
