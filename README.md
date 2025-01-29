# Traffic-Light

Traffic light state machine

## Assumptions

- Speed limit one a road is the same in both directions.
- If there is a vehicle detector, it is installed on every lane of the direction.

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

## Misc

If detector is not NONE:
    if distance is less than 80 this is a stop line detector, otherwise setback detector
