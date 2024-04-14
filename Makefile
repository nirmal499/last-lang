project-configure-fresh-with-unit-test:
	rm -fr build
	mkdir build
	cmake -B build -S . -DENABLE_TESTING=ON

project-configure:
    cmake -B build -S . -DENABLE_TESTING=ON

project-build:
    cmake --build build

project-build-target-exe:
    cmake --build build --target executable

project-build-target-unit-test:
    cmake --build build --target unit_tests

project-run-exe:
    ./build/lang/executable

project-run-test:
    ./build/tests/unit_tests