BUILD_DIR = build

.PHONY: setup build install clean

all: build

setup:
	meson setup $(BUILD_DIR)

build:
	meson compile -C $(BUILD_DIR)

install:
	sudo meson install -C $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
