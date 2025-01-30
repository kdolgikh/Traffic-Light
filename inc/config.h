#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_STR_LEN 64U
#define MAX_ROADS 2U               // Exactly 2 intersecting roads
#define MAX_DIRECTIONS 2U          // Each road has exactly 2 directions

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
    uint8_t count;       // Number of lanes
    bool is_protected;   // For left turn lanes - whether they have protected phase
    bool has_detector;   // Whether detector is installed
    detector_t detector; // Optional detector configuration
} lane_group_t;

typedef struct
{
    bool push_button; // Whether push button exists
    uint8_t distance; // Crossing distance in feet
} ped_crossing_t;

typedef struct
{
    direction_type_t type;
    bool has_left;            // Whether left turn exists
    bool has_right;           // Whether right turn exists
    bool has_ped_crossing;    // Whether crossing exists
    lane_group_t straight;    // Required straight lanes
    lane_group_t left;        // Optional left turn lanes
    lane_group_t right;       // Optional right turn lanes
    ped_crossing_t ped_cross; // Optional pedestrian crossing configuration
} direction_t;

#define ASSET_ID                \
    struct                      \
    {                           \
        char id[MAX_STR_LEN];   \
        char name[MAX_STR_LEN]; \
    }

typedef struct
{
    ASSET_ID;
    uint8_t speed_limit;
    direction_t directions[MAX_DIRECTIONS];
} road_t;

typedef struct
{
    ASSET_ID;
    intersection_type_t intersect_type;
    road_t roads[MAX_ROADS];
    road_t *main_road;
} config_t;

bool config_load(config_t *config, const char *filename);
bool config_validate(const config_t *const cfg_ptr);

#endif // CONFIG_H