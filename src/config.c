#include "config.h"
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define STRLEN(s) (sizeof(s) - 1U)

#define DET_TYPE_LOOP "LOOP"
#define DET_TYPE_VIDEO "VIDEO"
#define DET_TYPE_RADAR "RADAR"
#define MAX_DET_TYPE_STR_LEN MAX(                       \
    MAX(STRLEN(DET_TYPE_LOOP), STRLEN(DET_TYPE_VIDEO)), \
    STRLEN(DET_TYPE_RADAR))

#define DIR_TYPE_NB "NB"
#define DIR_TYPE_SB "SB"
#define DIR_TYPE_EB "EB"
#define DIR_TYPE_WB "WB"
#define MAX_DIR_TYPE_STR_LEN MAX(                  \
    MAX(STRLEN(DIR_TYPE_NB), STRLEN(DIR_TYPE_SB)), \
    MAX(STRLEN(DIR_TYPE_EB), STRLEN(DIR_TYPE_WB)))

typedef struct
{
    direction_type_t dir1;
    direction_type_t dir2;
} direction_pair_t;

typedef struct
{
    const uint8_t speed;
    const uint16_t distance;
} speed_table_entry_t;

typedef struct
{
    const char *const type_str;
    const size_t type_str_len;
    int type_enum_value;
} type_table_entry_t;

/* Config parameters parsing and validation function prototypes */

static bool parse_detector(json_object *det_obj, detector_t *const det_ptr);
static bool parse_lane_group(json_object *lane_obj, lane_group_t *const lane_ptr);
static bool parse_ped_crossing(json_object *ped_cross_obj, ped_crossing_t *const ped_cross_ptr);
static bool parse_direction(json_object *dir_obj, direction_t *const dir_ptr);
static bool parse_road(json_object *road_obj, road_t *const road_ptr);
static void str_to_upper(char *const str_ptr, size_t str_len);
static bool str_to_enum_type(
    const char *const param_str,
    char *const str,
    const size_t max_str_len,
    int *const enum_ptr,
    const type_table_entry_t *const table,
    const size_t table_size);
static bool str_to_direction_type(
    char *const dir_type_str,
    const size_t max_dir_type_str_len,
    direction_type_t *const dir_type_ptr);
static bool str_to_detector_type(
    char *const det_type_str,
    const size_t max_det_type_str_len,
    detector_type_t *const det_type_ptr);

/* Config validation function prototypes */

static bool is_turn_lane_allowed(const intersection_type_t type, const bool is_left);
static bool is_valid_directions_pair(const direction_t directions[MAX_DIRECTIONS]);
static bool validate_detector_config(
    const detector_t *const det_ptr,
    const bool is_main_road,
    const bool is_straight_lane,
    const uint8_t speed_limit);
static bool validate_direction_config(
    const intersection_type_t type,
    const direction_t *const dir_ptr,
    const bool is_main_road,
    const uint8_t speed_limit);
static bool validate_road_config(
    const intersection_type_t type,
    const road_t *const road_ptr,
    const bool is_main_road);

/* Config parameters parsing and validation */

static bool parse_detector(json_object *det_obj, detector_t *const det_ptr)
{
    bool result = false;

    do
    {
        if (det_obj == NULL || det_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in parse_detector\n");
            break;
        }

        json_object *temp_obj;

        // Parse detector type (required)
        if (!json_object_object_get_ex(det_obj, "type", &temp_obj))
        {
            fprintf(stderr, "Missing detector type\n");
            break;
        }

        char *det_type = (char *)json_object_get_string(temp_obj);

        if (!str_to_detector_type(det_type, MAX_DET_TYPE_STR_LEN, &det_ptr->type))
        {
            break;
        }

        // Parse detector installation distance (optional)
        if (json_object_object_get_ex(det_obj, "distance", &temp_obj))
        {
            det_ptr->distance = json_object_get_int(temp_obj);

            if (det_ptr->distance < MIN_DETECTOR_DISTANCE ||
                det_ptr->distance > MAX_DETECTOR_DISTANCE)
            {
                fprintf(stderr,
                        "Invalid setback detector distance %u, "
                        "must be within %u to %u feet\n",
                        det_ptr->distance,
                        MIN_DETECTOR_DISTANCE,
                        MAX_DETECTOR_DISTANCE);
                break;
            }
        }

        result = true;

    } while (0);

    return result;
}

static bool parse_lane_group(json_object *lane_obj, lane_group_t *const lane_ptr)
{
    bool result = false;

    do
    {
        if (lane_obj == NULL || lane_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in parse_lane_group\n");
            break;
        }

        json_object *temp_obj;

        // Parse lane count (required)
        if (!json_object_object_get_ex(lane_obj, "count", &temp_obj))
        {
            fprintf(stderr, "Missing lane count\n");
            break;
        }

        lane_ptr->count = json_object_get_int(temp_obj);

        if (lane_ptr->count < 1 || lane_ptr->count > MAX_LANES)
        {
            fprintf(stderr,
                    "Invalid lane count, must be a number between 1 and %u,"
                    "but got %u\n",
                    MAX_LANES,
                    lane_ptr->count);
            break;
        }

        // Parse protected phase (optional)
        if (json_object_object_get_ex(lane_obj, "protected", &temp_obj))
        {
            lane_ptr->is_protected = json_object_get_boolean(temp_obj);
        }

        // Parse detector configuration (optional)
        if (json_object_object_get_ex(lane_obj, "detector", &temp_obj))
        {
            if (parse_detector(temp_obj, &lane_ptr->detector))
            {
                lane_ptr->has_detector = true;
            }
            else
            {
                fprintf(stderr, "Failed to parse detector configuration\n");
                break;
            }
        }

        result = true;

    } while (0);

    return result;
}

static bool parse_ped_crossing(json_object *ped_cross_obj, ped_crossing_t *const ped_cross_ptr)
{
    bool result = false;

    do
    {
        if (ped_cross_obj == NULL || ped_cross_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in parse_ped_crossing\n");
            break;
        }

        json_object *temp_obj;

        // Parse push button (required)
        if (!json_object_object_get_ex(ped_cross_obj, "push_button", &temp_obj))
        {
            fprintf(stderr, "Missing push button\n");
            break;
        }

        ped_cross_ptr->push_button = json_object_get_boolean(temp_obj);

        // Parse distance (required)
        if (!json_object_object_get_ex(ped_cross_obj, "distance", &temp_obj))
        {
            fprintf(stderr, "Missing distance\n");
            break;
        }

        ped_cross_ptr->distance = json_object_get_int(temp_obj);

        if (ped_cross_ptr->distance < MIN_CROSSING_DISTANCE ||
            ped_cross_ptr->distance > MAX_CROSSING_DISTANCE)
        {
            fprintf(stderr,
                    "Invalid pedestrian crossing distance %u, "
                    "must be within %u to %u feet\n",
                    ped_cross_ptr->distance,
                    MIN_CROSSING_DISTANCE,
                    MAX_CROSSING_DISTANCE);
            break;
        }

        result = true;

    } while (0);

    return result;
}

static bool parse_direction(json_object *dir_obj, direction_t *const dir_ptr)
{
    bool result = false;

    do
    {
        if (dir_obj == NULL || dir_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in parse_direction\n");
            break;
        }

        json_object *temp_obj;

        // Parse direction type (required)
        if (!json_object_object_get_ex(dir_obj, "type", &temp_obj))
        {
            fprintf(stderr, "Missing direction type\n");
            break;
        }

        char *dir_type = (char *)json_object_get_string(temp_obj);

        if (!str_to_direction_type(dir_type, MAX_DIR_TYPE_STR_LEN, &dir_ptr->type))
        {
            break;
        }

        // Parse lanes (required)
        if (!json_object_object_get_ex(dir_obj, "lanes", &temp_obj))
        {
            fprintf(stderr, "Missing lanes\n");
            break;
        }

        json_object *lane_obj;

        // Parse straight lanes (required)
        if (!json_object_object_get_ex(temp_obj, "straight", &lane_obj) ||
            !parse_lane_group(lane_obj, &dir_ptr->straight))
        {
            fprintf(stderr, "Straight lane is missing or failed to parse it\n");
            break;
        }

        // Parse left turn lanes (optional)
        if (json_object_object_get_ex(temp_obj, "left", &lane_obj))
        {
            if (parse_lane_group(lane_obj, &dir_ptr->left))
            {
                dir_ptr->has_left = true;
            }
            else
            {
                fprintf(stderr, "Failed to parse left turn lanes\n");
                break;
            }
        }

        // Parse right turn lanes (optional)
        if (json_object_object_get_ex(temp_obj, "right", &lane_obj))
        {
            if (parse_lane_group(lane_obj, &dir_ptr->right))
            {
                dir_ptr->has_right = true;
            }
            else
            {
                fprintf(stderr, "Failed to parse right turn lanes\n");
                break;
            }
        }

        // Parse pedestrian crossing (optional)
        if (json_object_object_get_ex(dir_obj, "pedestrian", &temp_obj))
        {
            if (parse_ped_crossing(temp_obj, &dir_ptr->ped_cross))
            {
                dir_ptr->has_ped_crossing = true;
            }
            else
            {
                fprintf(stderr, "Failed to parse pedestrian crossing\n");
                break;
            }
        }

        result = true;

    } while (0);

    return result;
}

static bool parse_road(json_object *road_obj, road_t *const road_ptr)
{
    bool result = false;

    do
    {
        if (road_obj == NULL || road_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in parse_road\n");
            break;
        }

        json_object *temp_obj;

        // Parse road ID (required)
        if (!json_object_object_get_ex(road_obj, "id", &temp_obj))
        {
            fprintf(stderr, "Missing road ID\n");
            break;
        }

        snprintf(road_ptr->id, MAX_STR_LEN, "%s", json_object_get_string(temp_obj));

        // Parse road name (required)
        if (!json_object_object_get_ex(road_obj, "name", &temp_obj))
        {
            fprintf(stderr, "Missing road name\n");
            break;
        }

        snprintf(road_ptr->name, MAX_STR_LEN, "%s", json_object_get_string(temp_obj));

        // Parse speed limit (required)
        if (!json_object_object_get_ex(road_obj, "speed_limit", &temp_obj))
        {
            fprintf(stderr, "Missing speed limit\n");
            break;
        }

        road_ptr->speed_limit = json_object_get_int(temp_obj);

        if (road_ptr->speed_limit < MIN_SPEED_LIMIT ||
            road_ptr->speed_limit > MAX_SPEED_LIMIT)
        {
            fprintf(stderr,
                    "Invalid speed limit %u, "
                    "must be within %u to %u mph\n",
                    road_ptr->speed_limit,
                    MIN_SPEED_LIMIT,
                    MAX_SPEED_LIMIT);
            break;
        }

        // Parse directions array (required)
        if (!json_object_object_get_ex(road_obj, "directions", &temp_obj) ||
            json_object_array_length(temp_obj) != MAX_DIRECTIONS)
        {
            fprintf(stderr, "Missing or invalid directions array\n");
            break;
        }

        // Parse each direction (required)
        size_t i;
        for (i = 0; i < MAX_DIRECTIONS; i++)
        {
            if (!parse_direction(json_object_array_get_idx(temp_obj, i), &road_ptr->directions[i]))
            {
                break;
            }
        }

        if (i != MAX_DIRECTIONS)
        {
            fprintf(stderr, "Failed to parse direction %lu\n", i);
            break;
        }

        result = true;

    } while (0);

    return result;
}

static void str_to_upper(char *const str_ptr, size_t str_len)
{
    for (size_t i = 0; i < str_len; i++)
    {
        str_ptr[i] = (char)toupper(str_ptr[i]);
    }
}

static bool str_to_enum_type(
    const char *const param_str,
    char *const str,
    const size_t max_str_len,
    int *const enum_ptr,
    const type_table_entry_t *const table,
    const size_t table_size)
{
    if (param_str == NULL ||
        str == NULL ||
        max_str_len == 0 ||
        enum_ptr == NULL ||
        table == NULL ||
        table_size < sizeof(type_table_entry_t))
    {
        return false;
    }

    size_t str_len = strnlen(str, MAX_STR_LEN);

    if (str_len > max_str_len)
    {
        fprintf(
            stderr,
            "Invalid %s type length of %lu, "
            "must be not more than %lu characters\n",
            param_str,
            str_len,
            max_str_len);

        return false;
    }

    str_to_upper(str, str_len);

    for (size_t i = 0; i < table_size; i++)
    {
        if (str_len != table[i].type_str_len)
        {
            continue;
        }

        if (strncmp(str, table[i].type_str, MAX_STR_LEN) == 0)
        {
            *enum_ptr = table[i].type_enum_value;
            return true;
        }
    }

    fprintf(stderr, "Unknown %s type: %s\n", param_str, str);
    return false;
}

static bool str_to_direction_type(
    char *const dir_type_str,
    const size_t max_dir_type_str_len,
    direction_type_t *const dir_type_ptr)
{
    static const type_table_entry_t dir_types[] = {
        {DIR_TYPE_NB, STRLEN(DIR_TYPE_NB), DIRECTION_NB},
        {DIR_TYPE_SB, STRLEN(DIR_TYPE_SB), DIRECTION_SB},
        {DIR_TYPE_EB, STRLEN(DIR_TYPE_EB), DIRECTION_EB},
        {DIR_TYPE_WB, STRLEN(DIR_TYPE_WB), DIRECTION_WB}};

    return str_to_enum_type(
        "direction",
        dir_type_str,
        max_dir_type_str_len,
        (int *const)dir_type_ptr,
        dir_types,
        sizeof(dir_types) / sizeof(type_table_entry_t));
}

static bool str_to_detector_type(
    char *const det_type_str,
    const size_t max_det_type_str_len,
    detector_type_t *const det_type_ptr)
{
    static const type_table_entry_t det_types[] = {
        {DET_TYPE_LOOP, STRLEN(DET_TYPE_LOOP), DETECTOR_LOOP},
        {DET_TYPE_VIDEO, STRLEN(DET_TYPE_VIDEO), DETECTOR_VIDEO},
        {DET_TYPE_RADAR, STRLEN(DET_TYPE_RADAR), DETECTOR_RADAR}};

    return str_to_enum_type(
        "detector",
        det_type_str,
        max_det_type_str_len,
        (int *const)det_type_ptr,
        det_types,
        sizeof(det_types) / sizeof(type_table_entry_t));
}

/* Config Validation */

// TODO:
// - Add validation of pedestrian crossing time vs distance
// - Add validation of protected phase for left turn lanes

static bool is_turn_lane_allowed(const intersection_type_t type, const bool is_left)
{
    bool result;

    switch (type)
    {
    case INTERSECTION_TYPE_1: // straight only
        result = false;
        break;
    case INTERSECTION_TYPE_2: // straight + right
        result = !is_left;    // only right turns allowed
        break;
    case INTERSECTION_TYPE_3: // straight + left
        result = is_left;     // only left turns allowed
        break;
    case INTERSECTION_TYPE_4: // straight + left + right
        result = true;        // all turns allowed
        break;
    default:
        fprintf(stderr, "Unknown intersection type: %d\n", type);
        result = false;
        break;
    }

    return result;
}

static bool is_valid_directions_pair(const direction_t directions[MAX_DIRECTIONS])
{
    static const direction_pair_t valid_pairs[] = {
        {DIRECTION_NB, DIRECTION_SB},
        {DIRECTION_EB, DIRECTION_WB},
        // Add more valid pairs here if necessary, e.g.:
        // {DIRECTION_NE, DIRECTION_SW},
        // {DIRECTION_NW, DIRECTION_SE},
    };

    for (size_t i = 0; i < sizeof(valid_pairs) / sizeof(direction_pair_t); i++)
    {
        if ((directions[0].type == valid_pairs[i].dir1 &&
             directions[1].type == valid_pairs[i].dir2) ||
            (directions[0].type == valid_pairs[i].dir2 &&
             directions[1].type == valid_pairs[i].dir1))
        {
            return true;
        }
    }

    fprintf(stderr, "Invalid direction combination: %d and %d\n",
            directions[0].type, directions[1].type);

    return false;
}

static bool validate_detector_config(
    const detector_t *const det_ptr,
    const bool is_main_road,
    const bool is_straight_lane,
    const uint8_t speed_limit)
{
    bool result = false;

    do
    {
        if (det_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in validate_detector_config\n");
            break;
        }

        if (is_main_road)
        {
            if (det_ptr->distance == 0)
            {
                fprintf(stderr, "Main road cannot have a stop line detector\n");
                break;
            }

            if (is_straight_lane)
            {
                // Check if distance is appropriate for the detector type
                if (det_ptr->type == DETECTOR_LOOP)
                {
                    static const speed_table_entry_t loop_distances[] = {
                        {30U, 80U},
                        {35U, 200U},
                        {40U, 300U},
                        {45U, 330U},
                        {50U, 370U},
                        {55U, 445U},
                        {60U, 485U}};

                    size_t i;
                    for (i = 0; i < sizeof(loop_distances) / sizeof(speed_table_entry_t); i++)
                    {
                        if (speed_limit == loop_distances[i].speed)
                        {
                            if (det_ptr->distance != loop_distances[i].distance)
                            {
                                fprintf(stderr,
                                        "Invalid setback detector distance %u for speed limit %u, "
                                        "must be %u feet\n",
                                        det_ptr->distance,
                                        speed_limit,
                                        loop_distances[i].distance);
                                break;
                            }
                        }
                    }

                    if (i == sizeof(loop_distances) / sizeof(speed_table_entry_t))
                    {
                        fprintf(stderr,
                                "Invalid speed limit %u for setback detector on main road, "
                                "must be one of the following: 30, 35, 40, 45, 50, 55, 60 mph\n",
                                speed_limit);
                        break;
                    }
                }

                // Add more checks for other detector types if necessary
            }
            else
            {
                fprintf(stderr, "Main road cannot have a detector on a turn lane\n");
                break;
            }
        }
        else if (det_ptr->distance > 0)
        {
            fprintf(stderr, "Non-main roads cannot have a setback detector\n");
            break;
        }

        result = true;

    } while (0);

    return result;
}

static bool validate_direction_config(
    const intersection_type_t type,
    const direction_t *const dir_ptr,
    const bool is_main_road,
    const uint8_t speed_limit)
{
    bool result = false;

    do
    {
        if (dir_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in validate_direction_config\n");
            break;
        }

        if (dir_ptr->straight.has_detector)
        {
            if (!validate_detector_config(
                    &dir_ptr->straight.detector,
                    is_main_road,
                    true,
                    speed_limit))
            {
                fprintf(stderr, "Invalid straight lane detector configuration\n");
                break;
            }
        }

        if (dir_ptr->has_left)
        {
            if (!is_turn_lane_allowed(type, true))
            {
                fprintf(stderr, "Left turn lane not allowed for intersection type %d\n", type);
                break;
            }

            if (dir_ptr->left.has_detector)
            {
                if (!validate_detector_config(
                        &dir_ptr->left.detector,
                        is_main_road,
                        false,
                        speed_limit))
                {
                    fprintf(stderr, "Invalid left turn lane detector configuration\n");
                    break;
                }
            }
        }

        if (dir_ptr->has_right)
        {
            if (!is_turn_lane_allowed(type, false))
            {
                fprintf(stderr, "Right turn lane not allowed for intersection type %d\n", type);
                break;
            }

            if (dir_ptr->right.has_detector)
            {
                if (!validate_detector_config(
                        &dir_ptr->right.detector,
                        is_main_road,
                        false,
                        speed_limit))
                {
                    fprintf(stderr, "Invalid right turn lane detector configuration\n");
                    break;
                }
            }
        }

        result = true;

    } while (0);

    return result;
}

static bool validate_road_config(
    const intersection_type_t type,
    const road_t *const road_ptr,
    const bool is_main_road)
{
    bool result = false;

    do
    {
        if (road_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in validate_road_config\n");
            break;
        }

        size_t i;
        for (i = 0; i < MAX_DIRECTIONS; i++)
        {
            if (!validate_direction_config(
                    type,
                    &road_ptr->directions[i],
                    is_main_road,
                    road_ptr->speed_limit))
            {
                break;
            }
        }

        if (i != MAX_DIRECTIONS)
        {
            fprintf(stderr, "Invalid configuration for direction %lu\n", i);
            break;
        }

        result = true;

    } while (0);

    return result;
}

/*Interface Functions*/

bool config_load(config_t *config, const char *filename)
{
    bool status = false;
    json_object *root = NULL;

    do
    {
        if (config == NULL || filename == NULL)
        {
            fprintf(stderr, "Assertion error in load_config\n");
            break;
        }

        // Load JSON file
        root = json_object_from_file(filename);
        if (root == NULL)
        {
            fprintf(stderr, "Failed to load config file: %s\n", filename);
            break;
        }

        json_object *temp_obj;

        // Parse intersection ID (required)
        if (!json_object_object_get_ex(root, "intersection_id", &temp_obj))
        {
            fprintf(stderr, "Missing intersection_id\n");
            break;
        }

        snprintf(config->id, MAX_STR_LEN, "%s", json_object_get_string(temp_obj));

        // Parse intersection type (required)
        if (!json_object_object_get_ex(root, "type", &temp_obj))
        {
            fprintf(stderr, "Missing intersection type\n");
            break;
        }

        config->intersect_type = json_object_get_int(temp_obj) - 1; // Convert 1-based to 0-based enum

        if (config->intersect_type < INTERSECTION_TYPE_1 || config->intersect_type > INTERSECTION_TYPE_4)
        {
            fprintf(
                stderr,
                "Invalid intersection type, must be a number from 1 to 4, "
                "but got %d\n",
                config->intersect_type);
            break;
        }

        // Parse main road ID (optional)
        const char *main_road_id = NULL;
        if (json_object_object_get_ex(root, "main_road", &temp_obj))
        {
            main_road_id = json_object_get_string(temp_obj);
        }

        // Parse roads array (required)
        if (!json_object_object_get_ex(root, "roads", &temp_obj) ||
            (json_object_array_length(temp_obj) != MAX_ROADS))
        {
            fprintf(stderr, "Missing or invalid roads array\n");
            break;
        }

        // Parse each road
        json_object *road_obj;
        size_t i;
        for (i = 0; i < MAX_ROADS; i++)
        {
            road_obj = json_object_array_get_idx(temp_obj, i);

            if (!parse_road(road_obj, &config->roads[i]))
            {
                fprintf(stderr, "Failed to parse road %lu\n", i);
                break;
            }

            // Set main road pointer if this is the main road
            if (main_road_id != NULL &&
                strncmp(config->roads[i].id, main_road_id, MAX_STR_LEN) == 0)
            {
                config->main_road = &config->roads[i];
            }
        }

        if (i != MAX_ROADS)
        {
            fprintf(stderr, "Failed to parse road %lu\n", i);
            break;
        }

        if (main_road_id != NULL && config->main_road == NULL)
        {
            fprintf(stderr, "No matching entry for the main road found\n");
            break;
        }

        status = true;

    } while (0);

    if (root != NULL)
    {
        json_object_put(root);
    }

    return status;
}

bool config_validate(const config_t *const cfg_ptr)
{
    bool result = false;

    do
    {
        if (cfg_ptr == NULL)
        {
            fprintf(stderr, "Assertion error in config_validate\n");
            break;
        }

        bool found_duplicate = false;
        char seen_names[MAX_ROADS][MAX_STR_LEN] = {0};
        direction_pair_t seen_dir_pairs[MAX_ROADS] = {0};
        size_t i;
        for (i = 0; i < MAX_ROADS; i++)
        {
            // Check for duplicate road names and direction pairs
            for (size_t j = 0; j < i; j++)
            {
                if (strncmp(
                        seen_names[j],
                        cfg_ptr->roads[i].name,
                        MAX_STR_LEN) == 0)
                {
                    fprintf(
                        stderr,
                        "Duplicate road name found: %s\n",
                        cfg_ptr->roads[i].name);
                    found_duplicate = true;
                    break;
                }

                if ((seen_dir_pairs[j].dir1 == cfg_ptr->roads[i].directions[0].type &&
                     seen_dir_pairs[j].dir2 == cfg_ptr->roads[i].directions[1].type) ||
                    (seen_dir_pairs[j].dir2 == cfg_ptr->roads[i].directions[0].type &&
                     seen_dir_pairs[j].dir1 == cfg_ptr->roads[i].directions[1].type))
                {
                    fprintf(
                        stderr,
                        "Duplicate direction pair found for road %s\n",
                        cfg_ptr->roads[i].name);
                    found_duplicate = true;
                    break;
                }
            }

            if (found_duplicate)
            {
                break;
            }

            snprintf(seen_names[i], MAX_STR_LEN, "%s", cfg_ptr->roads[i].name);

            if (is_valid_directions_pair(cfg_ptr->roads[i].directions))
            {
                seen_dir_pairs[i].dir1 = cfg_ptr->roads[i].directions[0].type;
                seen_dir_pairs[i].dir2 = cfg_ptr->roads[i].directions[1].type;
            }
            else
            {
                fprintf(
                    stderr,
                    "Invalid direction combination for road %s\n",
                    cfg_ptr->roads[i].name);

                break;
            }

            bool is_main_road = (cfg_ptr->main_road == &cfg_ptr->roads[i]);

            if (!validate_road_config(
                    cfg_ptr->intersect_type,
                    &cfg_ptr->roads[i],
                    is_main_road))
            {
                fprintf(stderr, "Invalid configuration for road %lu\n", i);
                break;
            }
        }

        if (i != MAX_ROADS)
        {
            break;
        }

        result = true;

    } while (0);

    return result;
}
