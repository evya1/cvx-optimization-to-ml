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

.PHONY: build run clean rebuild test dev docker docker-run docker-clean docker-rebuild \
        msolve-test-real-roots-sec4p1 docker-run-msolve show-msolve-output-sec4p1

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

# ========== Testing & Formatting ==========

test:
	@echo "🧪 (TODO) Add test framework support"
	@echo "Running test stub..."
	$(CXX) -std=c++$(CXX_STD) $(TEST_DIR)/test_rotate.cpp -I$(INCLUDE_DIR) -o $(BUILD_DIR)/test_rotate && $(BUILD_DIR)/test_rotate

dev: test run


# ========== Docker Targets ==========

docker:
	@echo "🐳 Building Docker image..."
	docker build -t $(DOCKER_IMAGE) .

docker-run: docker
	@echo "🐋 Running in Docker..."
	docker run --rm --name $(DOCKER_CONTAINER) $(DOCKER_IMAGE)


docker-clean:
	@echo "🧹 Cleaning Docker image and stopped containers..."
	docker rm -f $(DOCKER_CONTAINER) 2>/dev/null || true
	docker rmi -f $(DOCKER_IMAGE) 2>/dev/null || true

docker-rebuild:
	$(MAKE) docker-clean
	$(MAKE) docker

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


msolve-run-%:
	$(call RUN_MSOLVE,$*.ms,$*.out)


help:
	@echo "Usage:"
	@echo "  make [target]"
	@echo ""
	@echo "Core Targets:"
	@echo "  build           Build the project using CMake"
	@echo "  run             Run the compiled binary"
	@echo "  clean           Remove build artifacts"
	@echo "  rebuild         Clean and rebuild"
	@echo ""
	@echo "Utility Targets:"
	@echo "  test            Run unit tests (if any)"
	@echo "  format          Format all source files"
	@echo ""
	@echo "Docker Targets:"
	@echo "  docker          Build Docker image"
	@echo "  docker-run      Run the image (default binary)"
	@echo "  docker-clean    Remove Docker containers/images"
	@echo "  msolve-test-real-roots-sec4p1  Run msolve on real root example from Section 4.1"
	@echo "  show-msolve-output-sec4p1      Show output result of the msolve test"

