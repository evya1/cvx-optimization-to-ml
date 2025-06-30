# ========== Configuration ==========
BUILD_DIR := build
TARGET := cvx-optimization-to-ml
SRC_DIR := src
INCLUDE_DIR := include
TEST_DIR := tests
BINARY := $(BUILD_DIR)/$(TARGET)


DOCKER_IMAGE := cvx-optimization-to-ml
DOCKER_CONTAINER := cvx-optimization-to-ml-runner

CMAKE_FLAGS := -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CXX_STD := 17
CXX := g++

MS_INPUT_DIR := ms_test_inputs
MS_OUTPUT_DIR := ms_outputs
MS_INPUT_FILE := $(MS_INPUT_DIR)/real_roots_sec4p1.ms
MS_OUTPUT_FILE := $(MS_OUTPUT_DIR)/real_roots_sec4p1.out

# ========== Core Targets ==========

.PHONY: all build run clean rebuild test dev help
.PHONY: docker docker-run docker-clean docker-rebuild docker-run-tests docker-shell
.PHONY: docker-run-msolve msolve-test-real-roots-sec4p1 show-msolve-output-sec4p1
.PHONY: msolve-run-% ci

# ========== Build & Run Targets ==========

all: build

build:
	@echo "🔨 Building project..."
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		cmake -B $(BUILD_DIR) -DCMAKE_CXX_STANDARD=$(CXX_STD) $(CMAKE_FLAGS); \
	fi
	cmake --build $(BUILD_DIR)

run: build
	@echo "🚀 Running $(TARGET)..."
	@echo ""
	./$(BUILD_DIR)/$(TARGET)

clean:
	@echo "🧼 Cleaning build files..."
	rm -rf $(BUILD_DIR)

rebuild: clean build

# ========== Development Utilities ==========

test: build
	@echo "🧪 Running all unit tests..."
	./$(BUILD_DIR)/test-runner

dev: test run


# ========== Docker Image & Container ==========

docker:
	@echo "🐳 Building Docker image..."
	docker build -t $(DOCKER_IMAGE) .

docker-run: docker
	@echo "🐋 Running in Docker..."
	docker run --rm --name $(DOCKER_CONTAINER) $(DOCKER_IMAGE)

docker-run-tests: docker
	@echo "🧪 Running tests inside Docker..."
	docker run --rm $(DOCKER_IMAGE) /test-runner

docker-shell: docker
	docker run --rm -it --entrypoint /bin/bash -v $(PWD):/app $(DOCKER_IMAGE)

docker-clean:
	@echo "🧹 Cleaning Docker image and stopped containers..."
	docker rm -f $(DOCKER_CONTAINER) 2>/dev/null || true
	docker rmi -f $(DOCKER_IMAGE) 2>/dev/null || true

docker-rebuild:
	$(MAKE) docker-clean
	$(MAKE) docker

# ========== msolve Integration ==========

docker-run-msolve:
	@mkdir -p ms_outputs
	docker run --rm \
		-v $(PWD)/ms_test_inputs:/inputs \
		-v $(PWD)/ms_outputs:/outputs \
		$(DOCKER_IMAGE) msolve -f /inputs/in.ms -o /outputs/out.ms

msolve-test-real-roots-sec4p1: docker
	@mkdir -p ms_test_inputs
	@mkdir -p ms_outputs
	@echo "📦 Running msolve on real_roots_sec4p1.ms..."
	docker run --rm \
		-v $(PWD)/ms_test_inputs:/inputs \
		-v $(PWD)/ms_outputs:/outputs \
		$(DOCKER_IMAGE) msolve \
		-f /inputs/real_roots_sec4p1.ms \
		-o /outputs/real_roots_sec4p1.out

show-msolve-output-sec4p1:
	@echo "🧾 Output from msolve:"
	@cat ms_outputs/real_roots_sec4p1.out

define RUN_MSOLVE
	@mkdir -p $(MS_OUTPUT_DIR)
	docker run --rm \
		-v $(PWD)/$(MS_INPUT_DIR):/inputs \
		-v $(PWD)/$(MS_OUTPUT_DIR):/outputs \
		$(DOCKER_IMAGE) msolve \
		-f /inputs/$(1) \
		-o /outputs/$(2)
endef

run-full-eq172:
	docker run --rm \
		-v $(PWD):/app \
		-w /app \
		$(DOCKER_IMAGE) \
		bash scripts/entrypoint.sh

run-pipeline-eq172:
	docker run --rm \
		-v $(PWD):/app \
		-w /app \
		$(DOCKER_IMAGE) \
		python3 scripts/run_eq172_pipeline.py

run-e2e-pipeline: build
	@echo "🚀 Starting full end-to-end pipeline..."
	@echo "1. [C++] Generating symbolic parameters (F, G, c) from 3D data..."
	docker run --rm \
		-v $(PWD):/app \
		-w /app/build \
		$(DOCKER_IMAGE) \
		./fg_to_json
	@echo "\n2. [Python] Running symbolic solver on generated parameters..."
	$(MAKE) run-pipeline-eq172
	@echo "\n✅ End-to-end pipeline complete."


msolve-run-%:
	$(call RUN_MSOLVE,$*.ms,$*.out)

# ========== Local Sanity Pipeline ==========

# Build and run C++ exporter to JSON
local-fg-json: build
	@echo "🔧 [Local] Running fg_to_json to export F, G, c..."
	./$(BUILD_DIR)/fg_to_json

# Run SymPy + msolve locally (you must have msolve installed natively)
local-solve:
	@echo "🧠 [Local] Running SymPy pipeline + msolve..."
	python3 scripts/run_eq172_pipeline.py

# Full local end-to-end run
run-local-pipeline: local-fg-json local-solve
	@echo "📄 [Local] Displaying symbolic equations (.ms):"
	cat ms_test_inputs/eq172_input.ms
	@echo "\n🧾 [Local] Displaying msolve output:"
	cat ms_outputs/eq172_input.out
	@echo "\n✅ [Local] End-to-end pipeline complete."


# ========== CI & Meta Targets ==========

ci: docker-rebuild run-pipeline-eq172 docker-run-tests docker-run
	@echo "✅ CI validation complete."

help:
	@echo "Usage:"
	@echo "  make [target]"
	@echo ""
	@echo "Build & Run:"
	@echo "  build           Build the project using CMake"
	@echo "  run             Run the compiled binary"
	@echo "  clean           Remove build artifacts"
	@echo "  rebuild         Clean and rebuild"
	@echo ""
	@echo "Testing:"
	@echo "  test            Run unit tests"
	@echo "  dev             Run unit tests and then main binary"
	@echo ""
	@echo "Docker:"
	@echo "  docker          Build Docker image"
	@echo "  docker-run      Run binary inside Docker"
	@echo "  docker-run-tests Run tests inside Docker"
	@echo "  docker-shell    Drop into an interactive shell inside the container"
	@echo "  docker-clean    Remove Docker container/image"
	@echo "  docker-rebuild  Clean and rebuild Docker image"
	@echo ""
	@echo "msolve:"
	@echo "  msolve-test-real-roots-sec4p1     Run msolve on predefined input"
	@echo "  show-msolve-output-sec4p1         Show msolve result output"
	@echo "  msolve-run-<name>                 Run msolve on a .ms file by name"
	@echo ""
	@echo "CI:"
	@echo "  ci              Full Docker-based validation (build + test + run)"

