.PHONY: build package clean test self-test release

build:
	cmake -B build -G "Unix Makefiles" && \
	cmake --build ./build

package: build
	cd build && make package

clean:
	rm -rf build
	rm -rf dist

test: build
	cd build && ctest

self-test:
	python3 self-test.py

release:
	python3 release.py