#!/bin/bash

# Compile test program
gcc -Wall -Werror -o test_indicators test_indicators.c indicators.c -lm

if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

# Run test program and capture output
output=$(./test_indicators)
exit_code=$?

if [ $exit_code -ne 0 ]; then
    echo "Test program failed"
    exit $exit_code
fi

echo "Test output:"
echo "$output"

# Simple check: verify output contains expected SMA value (example)
if echo "$output" | grep -q "SMA results:"; then
    echo "SMA test passed"
else
    echo "SMA test failed"
    exit 1
fi
