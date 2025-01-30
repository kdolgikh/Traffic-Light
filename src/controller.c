#include "controller.h"
#include "controller_type1.h"
#include <stdio.h>

bool controller_init(controller_t *const controller)
{
    bool result = false;

    do
    {
        if (controller == NULL || controller->config == NULL)
        {
            fprintf(stderr, "Assertion failed in controller_init\n");
            break;
        }

        if (controller->config->intersect_type == INTERSECTION_TYPE_1)
        {
            controller->init = controller_type1_init;
            controller->run = controller_type1_run;
        }
        else if (controller->config->intersect_type == INTERSECTION_TYPE_2 ||
                 controller->config->intersect_type == INTERSECTION_TYPE_3 ||
                 controller->config->intersect_type == INTERSECTION_TYPE_4)
        {
            fprintf(stderr, "Intersection type %d not yet implemented\n",
                    controller->config->intersect_type);
            break;
        }
        else
        {
            fprintf(stderr, "Unknown intersection type: %d\n",
                    controller->config->intersect_type);
            break;
        }

        if (controller->init == NULL || controller->run == NULL)
        {
            fprintf(stderr, "Controller functions not set\n");
            break;
        }

        result = controller->init(controller);

    } while (0);

    return result;
}
