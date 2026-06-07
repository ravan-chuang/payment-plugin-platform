#!/bin/bash

set -u

CONFIG_BACKUP="$(mktemp)"
cp plugins.conf "$CONFIG_BACKUP"

cleanup() {
    cp "$CONFIG_BACKUP" plugins.conf
    rm -f "$CONFIG_BACKUP"
}

trap cleanup EXIT

echo "========================================"
echo " E-Commerce Plugin System - Short Demo"
echo "========================================"
echo

echo "[1] Build project"
make >/dev/null
make bank_transfer >/dev/null
echo "Build completed."
echo

echo "[2] Plugin status management"
sed -i 's/paypal=enabled/paypal=disabled/' plugins.conf

echo "Enabled payment methods:"
./shop_app list
echo

echo "Attempt to use disabled PayPal:"
./shop_app pay paypal 2000 2>&1 |
grep -E "Error:|Method:"
echo

echo "Enable PayPal through plugins.conf"
sed -i 's/paypal=disabled/paypal=enabled/' plugins.conf

./shop_app pay paypal 2000 |
grep -E "Selected Method:|Payment Status:|Message:"
echo

echo "[3] Complete checkout flow"
./shop_app checkout festival_discount line_pay |
grep -E "Subtotal:|Discount Name:|Discount Amount:|Final Amount:|Payment Name:|Payment Status:|Checkout transaction"
echo

echo "[4] Plugin validation summary"
./shop_app validate |
grep -E "Passed:|Failed:|Overall Result:"
echo

echo "========================================"
echo " Short demo finished successfully"
echo "========================================"
