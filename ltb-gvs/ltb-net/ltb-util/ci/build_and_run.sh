#!/usr/bin/env bash

set -e # fail script if any individual commands fail

cmake -E make_directory docker-cmake-build

# Configure
cmake -E chdir docker-cmake-build \
  cmake .. \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DLTB_USE_DEV_FLAGS=ON \
  -DLTB_BUILD_TESTS="${BUILD_TESTS}"

# Build
cmake -E chdir docker-cmake-build cmake --build . --parallel

if [ "${BUILD_TESTS}" == "ON" ]; then
  # Run the tests
  cmake -E chdir docker-cmake-build ctest

  if [ "${BUILD_TYPE}" == "Debug" ] && [ "${CXX}" == "g++" ]; then
    # Generate coverage reports
    COVERAGE_TARGETS="$(find docker-cmake-build -maxdepth 1 -executable -type f -name 'test_*' -exec basename {} \; | sed 's/[^ ]* */&_coverage/g')"
    # shellcheck disable=SC2086
    cmake -E chdir docker-cmake-build cmake --build . --target ${COVERAGE_TARGETS}
  fi
fi
