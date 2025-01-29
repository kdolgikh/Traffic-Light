# Traffic-Light

Traffic light state machine

## Assumptions

- Speed limit on a road is the same in both directions.

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
