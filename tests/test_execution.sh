
#!/usr/bin/env bash

set -u

PASS=0
FAIL=0

check() {
    local name="$1"
    local condition="$2"

    if [[ "$condition" == "true" ]]; then
        echo "[PASS] $name"
        PASS=$((PASS + 1))
    else
        echo "[FAIL] $name"
        FAIL=$((FAIL + 1))
    fi
}

# Test 1: Normal command
output=$(./profiler echo "Hello OSSP" 2>&1)
status=$?

check "Echo command succeeds" "$([[ $status -eq 0 ]] && echo true || echo false)"
check "Command output is captured" \
    "$(grep -q 'Hello OSSP' <<< "$output" && echo true || echo false)"

# Test 2: Elapsed time
output=$(./profiler sleep 1 2>&1)
status=$?

elapsed=$(awk '/Elapsed time:/ {print $3; exit}' <<< "$output")

check "Sleep command succeeds" "$([[ $status -eq 0 ]] && echo true || echo false)"
check "Elapsed time is approximately one second" \
    "$(awk -v t="${elapsed:-0}" 'BEGIN {
        print (t >= 0.9 && t <= 3.0) ? "true" : "false"
    }')"

# Test 3: Nonzero command exit status
output=$(./profiler false 2>&1)
status=$?

check "Nonzero exit status is preserved" "$([[ $status -eq 1 ]] && echo true || echo false)"

# Test 4: Missing command
output=$(./profiler nonexistent_command 2>&1)
status=$?

check "Missing command returns status 127" \
    "$([[ $status -eq 127 ]] && echo true || echo false)"

check "Missing command reports execution error" \
    "$(grep -q 'execvp:' <<< "$output" && echo true || echo false)"

# Summary
echo
echo "========== TEST SUMMARY =========="
echo "Passed: $PASS"
echo "Failed: $FAIL"
echo "=================================="

if [[ $FAIL -ne 0 ]]; then
    exit 1
fi
