# === STAGE 1: Build stage ===
FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libeigen3-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -Bbuild -H. && cmake --build build

# === STAGE 2: Runtime stage ===
FROM debian:bookworm-slim

COPY --from=builder /app/build/cvx-optimization-to-ml /cvx-optimization-to-ml

CMD ["/cvx-optimization-to-ml"]