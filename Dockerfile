FROM debian:12

RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    git \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m developer

WORKDIR /home/developer

USER developer

CMD ["/bin/bash"]
