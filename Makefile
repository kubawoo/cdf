BUILD_DIR ?= build

all: build

.PHONY: build
build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

.PHONY: test
test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: install
install: build
	cmake --install $(BUILD_DIR)
