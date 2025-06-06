# 🧠 cvx-optimization-to-ml

**C++ Convex Optimization and Polynomial System Solver with Dockerized Symbolic Computation via `msolve`**

This project is a modular C++ pipeline designed for convex optimization and algebraic computation, integrating symbolic solvers like [`msolve`](https://msolve.lip6.fr/) to compute real roots of polynomial systems within Docker. It uses Eigen for linear algebra operations and supports clean reproducible builds via CMake and Make.

---

## 📦 Features

-   🧮 Symbolic solution of multivariate polynomial systems via msolve (Docker-integrated).
-   🌀 Modular structure with Eigen-powered vector/matrix manipulation.
-   🐳 Full Docker environment for reproducible builds and isolated math toolchain (FLINT, GMP, MPFR).
-   🛠️ Developer-friendly `Makefile` with helper targets for running, testing, and Docker workflows.

---

## 🚀 Quick Start Guide

### ✅ Requirements

Make sure the following tools are installed **on your host machine**:

-   [Docker Desktop](https://www.docker.com/products/docker-desktop/)
-   [Git](https://git-scm.com/)
-   Optionally: `make` and `cmake` (if you want to build outside Docker)

---

### 📥 Clone the Repository

```bash
git clone https://github.com/evya1/cvx-optimization-to-ml.git
cd cvx-optimization-to-ml
```

---

### 🐳 Build and Run via Docker (Recommended)

#### 1. Build the Docker Image

```bash
make docker
```

This builds a two-stage image:

-   Stage 1 compiles FLINT and msolve from source.
-   Stage 2 produces a minimal runtime image with your binary and tools installed.

#### 2. Run the Main Program (Binary)

```bash
make docker-run
```

This runs `/cvx-optimization-to-ml` inside the container.

#### 3. Run msolve on an Example

```bash
make msolve-test-real-roots-sec4p1
make show-msolve-output-sec4p1
```

This will:

-   Run `msolve` on `ms_test_inputs/real_roots_sec4p1.ms`
-   Output the result to `ms_outputs/real_roots_sec4p1.out`
-   Print the content to your terminal

---

## 🛠️ Developer Workflow (Local Build)

### 1. Build Locally

```bash
make build
```

### 2. Run the Program

```bash
make run
```

### 3. Run Tests (optional)

```bash
make test
```

> ⚠️ Currently a placeholder – you'll need to implement test cases under `/tests`.

---

## 🧪 Directory Structure

```text
.
├── CMakeLists.txt                # CMake project definition
├── Dockerfile                    # Multi-stage Dockerfile
├── Makefile                      # All build + docker logic
├── include/                      # Header files (rotate.hpp)
├── src/                          # Main source code
│   ├── main.cpp
│   └── rotate.cpp
├── ms\_test\_inputs/              # Sample .ms file for msolve
│   └── real\_roots\_sec4p1.ms
├── ms\_outputs/                  # Output of msolve runs
│   └── real\_roots\_sec4p1.out
├── build/ (or cmake-build-debug/) # Local CMake build artifacts
└── tests/                       # (Placeholder) test files
```

---

## 📚 Technologies Used

-   **C++17**, **Eigen** – Linear Algebra
-   **FLINT**, **GMP**, **MPFR** – Number theory / multiprecision
-   **msolve** – Real root solver (symbolic polynomial systems)
-   **CMake** – Build configuration
-   **Docker** – Isolated build + runtime
-   **Makefile** – Workflow automation

---

## 🧠 Example Problem Solved

The provided `real_roots_sec4p1.ms` solves the following polynomial system:

```text
x + 2y + 2z = 1
x^2 + 2y^2 + 2z^2 = x
2xy + 2yz = y
```

Run `make msolve-test-real-roots-sec4p1` to compute its real solutions.

---

## 🤝 Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you'd like to change or add.

---

## 📝 License

MIT License (or specify your own).

---

## 🙌 Acknowledgments

-   [msolve authors](https://msolve.lip6.fr/)
-   [Eigen](https://eigen.tuxfamily.org/)
-   [FLINT](http://flintlib.org/)
