#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_STR_LEN 64
#define MAX_ROADS 2      // Exactly 2 intersecting roads
#define MAX_DIRECTIONS 2 // Each road has exactly 2 directions
#define MAX_LANES 3      // Maximum lanes per direction

typedef enum
{
    DETECTOR_NONE,
    DETECTOR_LOOP,
    DETECTOR_VIDEO,
    DETECTOR_RADAR
} detector_type_t;

typedef enum
{
    DIRECTION_NB,
    DIRECTION_SB,
    DIRECTION_EB,
    DIRECTION_WB
} direction_type_t;

typedef enum
{
    INTERSECTION_TYPE_1, // straight
    INTERSECTION_TYPE_2, // straight, right
    INTERSECTION_TYPE_3, // straight, left
    INTERSECTION_TYPE_4  // straight, left, right
} intersection_type_t;

typedef struct
{
    detector_type_t type;
    uint16_t distance; // Only used for setback detectors
} detector_t;

typedef struct
{
    int count;           // Number of lanes
    bool is_protected;   // For left turn lanes - whether they have protected phase
    detector_t detector; // Optional detector configuration
} lane_group_t;

typedef struct
{
    bool has_crossing; // Whether crossing exists
    bool push_button;  // Whether push button exists
    uint8_t distance;  // Crossing distance in feet
} ped_crossing_t;

typedef struct
{
    direction_type_t type;
    lane_group_t straight;
    lane_group_t left;  // Optional left turn lanes
    lane_group_t right; // Optional right turn lanes
    ped_crossing_t ped; // Optional pedestrian crossing configuration
} direction_t;

typedef struct
{
    char id[MAX_STR_LEN];
    char name[MAX_STR_LEN];
} asset_id_t;

typedef struct
{
    asset_id_t id;
    uint8_t speed_limit;
    direction_t directions[MAX_DIRECTIONS];
} road_t;

typedef struct
{
    asset_id_t id;
    intersection_type_t type;
    road_t roads[MAX_ROADS];
    road_t *const main_road;
} config_t;

bool load_config(config_t *config, const char *filename);

#endif // CONFIG_H