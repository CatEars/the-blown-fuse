.PHONY: build package clean test self-test release testall debug release

build-release:
	cmake -B build/release -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build build/release

build-debug:
	cmake -B build/debug -S . -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug

package: build-release
	cd build/release && make package

clean:
	rm -rf build
	rm -rf dist

test: build-debug
	cd build/debug && ctest

testall: self-test test

self-test:
	python3 self-test.py

release:
	python3 release.py