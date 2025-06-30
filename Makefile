DOCKER_IMAGE = cvx-optimization-to-ml
BUILD_DIR = build

.PHONY: all build clean test dev run docker-build docker-shell docker-run \
		docker-rebuild docker-run-tests msolve-build msolve-shell \
		run-e2e-pipeline run-pipeline-eq172 local-fg-json local-solve \
		run-local-pipeline ci help

all: build

# ========== Local C++ Development ==========

build:
	@echo "🔨 Building project..."
	cmake -B $(BUILD_DIR) -S .
	cmake --build $(BUILD_DIR)

test: build
	@echo "🧪 Running all unit tests..."
	./$(BUILD_DIR)/test-runner

clean:
	@echo "🧹 Cleaning local build directory..."
	rm -rf $(BUILD_DIR)

run: build
	@echo "▶️ Running main C++ executable..."
	./$(BUILD_DIR)/cvx-optimization-to-ml

dev: test run

# ========== Dockerized Workflow ==========

docker-build:
	@echo "🐳 Building Docker image..."
	docker build -t $(DOCKER_IMAGE) .

docker-rebuild:
	@echo "🐳 Rebuilding Docker image from scratch..."
	docker build --no-cache -t $(DOCKER_IMAGE) .

docker-run-tests:
	@echo "🧪 Running tests inside Docker..."
	docker run --rm -it $(DOCKER_IMAGE) /app/build/test-runner

docker-shell:
	@echo "🐚 Opening interactive shell in container..."
	docker run --rm -it \
		-v $(PWD):/app \
		-w /app \
		$(DOCKER_IMAGE) \
		bash

# ========== End-to-End Pipelines ==========

run-e2e-pipeline:
	@echo "🚀 Starting full end-to-end pipeline (in Docker)..."
	@echo "1. [C++] Generating symbolic parameters (F, G, c)..."
	docker run --rm \
		-v $(PWD)/inputs:/app/inputs \
		-v $(PWD)/ms_test_inputs:/app/ms_test_inputs \
		-v $(PWD)/ms_outputs:/app/ms_outputs \
		-w /app/build \
		$(DOCKER_IMAGE) \
		./fg_to_json
	@echo "\n2. [Python] Running symbolic solver on generated parameters..."
	docker run --rm \
		-v $(PWD)/inputs:/app/inputs \
		-v $(PWD)/ms_test_inputs:/app/ms_test_inputs \
		-v $(PWD)/ms_outputs:/app/ms_outputs \
		-w /app \
		$(DOCKER_IMAGE) \
		python3 scripts/run_eq172_pipeline.py
	@echo "\n✅ End-to-end pipeline complete."


local-fg-json: build
	@echo "🔧 [Local] Running fg_to_json to export F, G, c..."
	cd $(BUILD_DIR) && ./fg_to_json

local-solve:
	@echo "🧠 [Local] Running SymPy pipeline + msolve..."
	python3 scripts/run_eq172_pipeline.py

run-local-pipeline: local-fg-json local-solve
	@echo "📄 [Local] Displaying symbolic equations (.ms):"
	@cat ms_test_inputs/eq172_input.ms
	@echo "\n🧾 [Local] Displaying msolve output:"
	@cat ms_outputs/eq172_input.out
	@echo "\n✅ [Local] End-to-end pipeline complete."


# ========== CI & Meta Targets ==========

ci: docker-rebuild run-e2e-pipeline
	@echo "✅ CI validation complete."

help:
	@echo "Available commands:"
	@echo "  make build              - Build the C++ project locally"
	@echo "  make test               - Run C++ unit tests locally"
	@echo "  make run                - Run the main C++ executable locally"
	@echo "  make clean              - Remove local build artifacts"
	@echo "  make docker-rebuild     - Build/rebuild the production Docker image"
	@echo "  make run-e2e-pipeline   - Run the full C++/Python pipeline inside Docker"
	@echo "  make run-local-pipeline - Run the full pipeline on your local machine"
	@echo "  make ci                 - Run the full continuous integration check"
	@echo "  make docker-shell       - Open an interactive shell in the container"
