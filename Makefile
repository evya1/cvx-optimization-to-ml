# ========== Configuration ==========
BUILD_DIR := build
TARGET := cvx-optimization-to-ml
SRC_DIR := src
INCLUDE_DIR := include
TEST_DIR := tests

DOCKER_IMAGE := cvx-optimization-to-ml
DOCKER_CONTAINER := cvx-optimization-to-ml-runner

CMAKE_FLAGS := -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CXX_STD := 17
CXX := g++

# ========== Core Targets ==========

.PHONY: all build run clean rebuild test docker docker-run docker-clean help

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
