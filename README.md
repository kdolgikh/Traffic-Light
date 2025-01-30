# Traffic-Controller

Traffic controller state machine

## TODO

- [ ] Add missing config parameters:
  - [ ] Non-lock memory operation
  - [ ] Lock memory operation
  - [ ] Presence mode operation
- [ ] TODOs in code

## Design Requirements

### Intersections

- Exactly two roads per intersection
- Intersections involving one-way streets are not considered
- Intersections have unique IDs to enable potential identification when connected in a network

### Roads

- Roads have unique IDs, while names may be duplicated
- One road may be designated as main road
- Each road has exactly 2 directions (opposing traffic)
- Speed limits: 20-60 mph
- The speed limit remains the same in both directions.

### Lanes

- Maximum 3 lanes per direction
- Types of lanes:
  - Straight (through) lanes: Required
  - Left turn lanes: Optional
  - Right turn lanes: Optional
- Left turn phases require:
  - Dedicated left turn lane
  - Protected/permissive or protected-only operation
  - Protected-only required for dual left turns

### Detection

- Stop line detection:
  - Required for side street approaches
  - Required for left turn lanes
  - Non-lock memory operation
  - Presence mode operation
- Setback detection:
  - Required for main street through movements
  - Required for speeds ≥ 40 mph
  - Distance from stop line based on speed:
    - 30 mph: 80 ft
    - 35 mph: 200 ft
    - 40 mph: 300 ft
    - 45 mph: 330 ft
    - 50 mph: 370 ft
    - 55 mph: 445 ft
    - 60 mph: 485 ft
  - Lock memory operation
  - Presence mode operation

### Pedestrian crossing

- Optional on any approach
- When present requires:
  - Push buttons
  - Minimum walk time: 4 seconds with push buttons
  - Don't Walk time: Based on 3.5 ft/sec walking speed

### Signal timings

- Yellow clearance: 3-6 seconds
- Red clearance: 1.5-3 seconds
- Minimum green time:
  - 8 seconds for side street/left turns with stop line detection
  - 12-15 seconds for main street with setback detection
- Maximum green time:
  - Greater of:
    - Sum of Walk + Don't Walk times
    - Sum of Time Before Reduce + Time to Reduce + 15 seconds
  - Typically 40-60 seconds

### Phasing

- Through movements: Even phase numbers (2,4,6,8)
- Left turn movements: Odd phase numbers (1,3,5,7)
- Main street typically phases 2 and 6
- Phase 2 used for eastbound or southbound
- Phases numbered clockwise around intersection
- Concurrent pedestrian movements with parallel vehicle phase

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
