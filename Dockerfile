# === STAGE 1: Build Stage ===
FROM debian:bookworm-slim AS builder

# System dependencies
RUN apt-get update && apt-get install -y \
    build-essential cmake git curl unzip \
    autoconf automake libtool \
    libgmp-dev libmpfr-dev \
    libeigen3-dev \
    python3 python3-pip \
    python3-dev libffi-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY scripts/requirements.txt /app/scripts/requirements.txt

RUN cat /app/scripts/requirements.txt

RUN pip3 install -v --break-system-packages -r /app/scripts/requirements.txt


RUN git clone --branch v3.5.4 https://github.com/catchorg/Catch2.git /opt/catch2 \
    && cd /opt/catch2 \
    && cmake -Bbuild -H. \
    && cmake --build build --target install


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

# <<< CHANGE 1: Run your tests HERE, in the builder stage.
# If the tests fail, the docker build process will stop.
RUN /app/build/test-runner

# === STAGE 2: Runtime Stage ===
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libgmp10 libmpfr6 libgomp1 && apt-get clean && rm -rf /var/lib/apt/lists/*

# Copy final binary and msolve tool
COPY --from=builder /app/build/cvx-optimization-to-ml /cvx-optimization-to-ml
COPY --from=builder /usr/local/lib/libmsolve* /usr/local/lib/
COPY --from=builder /usr/local/bin/msolve /usr/local/bin/msolve
COPY --from=builder /usr/local/lib/libflint* /usr/local/lib/
ENV LD_LIBRARY_PATH=/usr/local/lib
RUN ldconfig


# Default run command
CMD ["/cvx-optimization-to-ml"]