FROM ubuntu:22.04

RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++-multilib \
    gcc-multilib \
    libsqlite3-dev:i386 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

CMD sh -c "mkdir -p build_linux && cd build_linux && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --config Release -j$(nproc)"