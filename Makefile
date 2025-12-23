.PHONY: build package clean

build:
	cmake -B build -G "Unix Makefiles" && \
	cmake --build ./build

package: build
	cd build && make package

clean:
	rm -rf build

