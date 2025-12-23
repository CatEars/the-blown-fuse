.PHONY: build clean

build:
	cmake -B build -G "Unix Makefiles" && \
	cmake --build ./build

clean:
	rm -rf build

