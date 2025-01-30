#ifndef CONTROLLER_TYPE1_H
#define CONTROLLER_TYPE1_H

/* Type 1 controller (straight paths only) */

#include "controller.h"

bool controller_type1_init(controller_t *const controller);
void controller_type1_run(controller_t *const controller);

#endif // CONTROLLER_TYPE1_H
