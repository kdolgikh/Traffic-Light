#include "config.h"
#include <stdio.h>
#include <stdlib.h>

static config_t config;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <config_file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!load_config(&config, argv[1]))
    {
        fprintf(stderr, "Failed to load configuration\n");
        return EXIT_FAILURE;
    }

    // TODO: run traffic light controller

    return EXIT_SUCCESS;
}
