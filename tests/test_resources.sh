#!/bin/bash
set -e

echo "[TEST] Checking resource report..."
output=$(./profiler sleep 1)

if echo "$output" | grep -q "Elapsed time:" &&
   echo "$output" | grep -q "User CPU time:" &&
   echo "$output" | grep -q "System CPU time:"; then
    echo "[PASS] Resource report contains timing and CPU fields"
else
    echo "[FAIL] Resource report is missing expected fields"
    exit 1
fi

echo "[PASS] Resource test completed"
