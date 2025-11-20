#!/bin/sh
set -euxo pipefail

c++ -std=c++23 -Wall -Wextra -pedantic main.cc -o out -lraylib -O3 -fsanitize=address,undefined
