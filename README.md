# Traffic-Light

Traffic light state machine

## Assumptions

The foundation of the traffic light state machine project is a basic four-way intersection with signals for all directions. The primary traffic flow is along the East-West direction, while the North-South direction serves as the secondary route. The signals for the secondary direction are actuated using metal-detecting loop sensors. Each signal consists of three sections: red, yellow, and green, operating in accordance with South Carolina traffic regulations.

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
