#!/bin/bash

set -u

PLUGIN_PATH="build/plugins/bank_transfer.so"

echo "========================================"
echo " Runtime Plugin Extension Demo"
echo "========================================"
echo

echo "[1] Main application build time"
stat -c "%y %n" shop_app
echo

echo "[2] Remove Bank Transfer plugin"
rm -f "$PLUGIN_PATH"
echo

echo "[3] Existing application attempts to load missing plugin"
./shop_app pay bank_transfer 1500 || true
echo

echo "[4] Build only Bank Transfer plugin"
make build/plugins/bank_transfer.so
echo

echo "[5] Load the newly deployed plugin without rebuilding shop_app"
./shop_app pay bank_transfer 1500
echo

echo "[6] Compare timestamps"
stat -c "%y %n" shop_app "$PLUGIN_PATH"
echo

echo "Conclusion:"
echo "bank_transfer.so was created after shop_app,"
echo "but the existing application loaded it successfully."
