# === STAGE 1: Build Stage ===
FROM debian:bookworm-slim AS builder

# System dependencies
RUN apt-get update && apt-get install -y \
    build-essential cmake git curl unzip \
    autoconf automake libtool \
    libgmp-dev libmpfr-dev \
    libeigen3-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# === Build FLINT 2.x ===
WORKDIR /opt
RUN git clone https://github.com/wbhart/flint2.git \
    && cd flint2 \
    && ./bootstrap.sh \
    && ./configure --with-gmp --with-mpfr \
    && make -j$(nproc) \
    && make install


# === Build MSOLVE (static) ===
WORKDIR /opt
RUN curl -L https://msolve.lip6.fr/downloads/v0.8.0/msolve-0.8.0.tar.gz -o msolve.tar.gz \
    && tar -xzf msolve.tar.gz && rm msolve.tar.gz \
    && cd msolve-0.8.0 \
    && ./configure --disable-shared --enable-static --prefix=/usr/local \
    && make -j$(nproc) \
    && cp ./msolve /usr/local/bin/msolve

# === Build C++ project ===
WORKDIR /app
COPY . .
RUN cmake -Bbuild -H. && cmake --build build

# === STAGE 2: Runtime Stage ===
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libgmp-dev libmpfr-dev libgomp1 && apt-get clean

# Copy final binary and msolve tool
COPY --from=builder /app/build/cvx-optimization-to-ml /cvx-optimization-to-ml
COPY --from=builder /usr/local/lib/libmsolve* /usr/local/lib/
COPY --from=builder /usr/local/bin/msolve /usr/local/bin/msolve
COPY --from=builder /usr/local/lib/libflint* /usr/local/lib/
ENV LD_LIBRARY_PATH=/usr/local/lib
RUN ldconfig


# Default run command
CMD ["/cvx-optimization-to-ml"]
