.PHONY: build package clean test

build:
	cmake -B build -G "Unix Makefiles" && \
	cmake --build ./build

package: build
	cd build && make package

clean:
	rm -rf build

test: build
	cd build && ctest