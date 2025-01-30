#include "controller.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

static volatile bool running = true;
static controller_t controller;

static void signal_handler(int signum)
{
    (void)signum; // Unused parameter
    running = false;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <config_file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Set up signal handling for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Load and validate configuration
    static config_t config;
    memset(&config, 0, sizeof(config));

    if (!config_load(&config, argv[1]))
    {
        fprintf(stderr, "Failed to load configuration\n");
        return EXIT_FAILURE;
    }

    if (!config_validate(&config))
    {
        fprintf(stderr, "Invalid configuration\n");
        return EXIT_FAILURE;
    }

    // Initialize controller
    if (!controller_init(&controller, &config))
    {
        fprintf(stderr, "Failed to initialize controller\n");
        return EXIT_FAILURE;
    }

    // Main control loop
    printf("Started traffic light controller. Press Ctrl+C to exit.\n");
    while (running)
    {
        controller_run(&controller);
    }

    return EXIT_SUCCESS;
}
