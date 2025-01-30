# Traffic-Controller

Traffic controller state machine

## TODO

- [ ] Change schema to match the exact type of intersection
  - [ ] Discuss possible ways to expand this schema for other types or use different schemas for different types
- [ ] Based on type, choose config parser code
- [ ] Add missing config parameters:
  - [ ] Non-lock memory operation
  - [ ] Lock memory operation
  - [ ] Presence mode operation
- [ ] TODOs in code

## Design Specifications

Specifications follow SC DOT design guidelines.

### Intersections

- Exactly two roads per intersection
- Intersection has a unique ID and type
- Intersection type defines its configuration and operation
- Roads in an intersection have unique names
- No pedestrian crossings
- One road is designated as a main road, another a side road

### Roads

- Each road has exactly 2 directions (opposing traffic)
- Speed limits: 20-60 mph
- The speed limit remains the same in both directions
- Grades are not accounted for (grade 0)

### Lanes

- Two straight lanes for through and right turn movements on main road
- One straight lane on side road
- No separate signal for right turn lanes
- A single left turn lane in each direction operating in permitted mode

### Phasing

- Four phases (2,4,6,8)
- Main road phases 2 and 6
- No overlap, concurrent, or split phases

### Detection

- Stop line detection:
  - Required for side road approaches
    - Non-lock memory operation
  - Not required for main road approaches
- Setback detection:
  - Not required for main road through movements

### Signal timings

- Yellow clearance: 3-6 seconds
- Red clearance: 1.5-3 seconds
- Minimum green time:
  - 8 seconds for side road
  - Green rest
- Maximum green time:
  - TBD

## Implementation Notes

- The design defines a pattern for the controller implementation based on the requirements outlined above, while leaving most features unimplemented, with only one or two provided as examples.
- JSON is chosen for the controller configuration due to the following advantages:
  - It supports a schema-based configuration pattern, providing guidance for config creators.
  - It enables validation against the schema using online or other tools without executing it on the controller.
  - It facilitates complex configurations, including conditional requirements, while minimizing data repetition.
  - In the future, once a stable JSON Schema validator written in C/C++ becomes available, the custom validation can be replaced with it.

## Environment

1. Build the image:

    ```bash
    docker build -t traffic-ctrl:0.1.0 .
    ```

2. Run the container:

    ```bash
    docker run -it --name ctr-traffic-ctrl --mount type=bind,src=/home/builder/github_repos/Traffic-Light,dst=/home/developer/Traffic-Light traffic-ctrl:0.1.0
    ```

## Testing
