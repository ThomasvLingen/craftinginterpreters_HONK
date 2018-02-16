#! /usr/bin/env bash
# Counts lines for a C++ project in the most simplistic way possible.

if command -v cloc; then
	cloc ./src
else
	wc -l `find ./src -name "*.cpp" -or -name "*.hpp"`
fi

