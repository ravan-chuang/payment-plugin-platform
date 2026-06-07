#!/bin/bash

set -u

BROKEN_SOURCE="tests/broken_payment.c"
BROKEN_PLUGIN="build/plugins/broken_payment.so"
CONFIG_ENTRY="broken_payment=enabled=./build/plugins/broken_payment.so"

cleanup() {
    sed -i '/^broken_payment=/d' plugins.conf
    rm -f "$BROKEN_PLUGIN"
}

trap cleanup EXIT

echo "========================================"
echo " Plugin Validator Demo"
echo "========================================"
echo

echo "[1] Validate all correct plugins"
cleanup
./shop_app validate
echo

echo "[2] Deploy intentionally broken plugin"
gcc -fPIC -shared "$BROKEN_SOURCE" -o "$BROKEN_PLUGIN"
echo "$CONFIG_ENTRY" >> plugins.conf
echo

echo "[3] Validator detects missing process_payment symbol"
./shop_app validate
echo

echo "[4] Remove broken plugin and validate again"
cleanup
./shop_app validate
echo

echo "Validator demo finished."
