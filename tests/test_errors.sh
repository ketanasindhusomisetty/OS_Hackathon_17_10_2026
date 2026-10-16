#!/bin/bash
set -e

echo "[TEST] Checking missing command handling..."
output=$(./profiler command_that_does_not_exist 2>&1 || true)

if echo "$output" | grep -q "Cannot execute"; then
    echo "[PASS] Missing command reports execution error"
else
    echo "[FAIL] Missing command error not reported"
    exit 1
fi

echo "[TEST] Checking nonzero exit status..."
./profiler sh -c 'exit 5' >/dev/null || status=$?
if [ "${status:-0}" -eq 5 ]; then
    echo "[PASS] Nonzero exit status preserved"
else
    echo "[FAIL] Expected exit status 5"
    exit 1
fi
