#!/usr/bin/env bash

set -e # fail script if any individual commands fail

# All compiled dependencies are installed in ${TRAVIS_BUILD_DIR}/deps/
DEPS_DIR="${TRAVIS_BUILD_DIR}/deps"
mkdir -p "${DEPS_DIR}" && cd "${DEPS_DIR}"

# Install a recent CMake
CMAKE_VERSION=$1
CMAKE_URL="https://cmake.org/files/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.0-Linux-x86_64.tar.gz"
mkdir cmake && travis_retry wget --no-check-certificate --quiet -O - "${CMAKE_URL}" | tar --strip-components=1 -xz -C cmake

export PATH=${DEPS_DIR}/cmake/bin:${PATH}

cmake --version
