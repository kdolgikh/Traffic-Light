#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <config_file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    static config_t config;
    memset(&config, 0, sizeof(config));

    if (!load_config(&config, argv[1]))
    {
        fprintf(stderr, "Failed to load configuration\n");
        return EXIT_FAILURE;
    }

    // TODO: run traffic light controller

    return EXIT_SUCCESS;
}
