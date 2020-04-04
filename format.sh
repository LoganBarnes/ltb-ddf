#!/usr/bin/env sh
FILE_LIST="$(find . -type f -name '*.cpp' -o -name '*.hpp' -o -name '*.cu' -o -name '*.cuh' | grep -v cmake-build)"
#echo "Formatting: ${FILE_LIST}"
# shellcheck disable=SC2086
clang-format-8 -i -style=file ${FILE_LIST}
