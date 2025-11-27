#!/bin/sh
set -euxo pipefail

just -f ./tdrf/Justfile
c++ -std=c++23 -Wall -Wextra -pedantic main.cc -o out -lraylib -O3 ./tdrf/build/libtdrf.a # -fsanitize=address,undefined
