#!/bin/bash
# src
find ../../src -type f \( -iname \*.?pp \) -exec clang-format -i {} \;
# apps
find ../../app -type f \( -iname \*.?pp \) -exec clang-format -i {} \;
