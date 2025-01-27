FROM debian:12

RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    git \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /traffic-controller

RUN useradd -m developer && \
    chown -R developer:developer /traffic-controller

USER developer

CMD ["/bin/bash"]
