#!/bin/bash

CONFIG_BACKUP="$(mktemp)"
cp plugins.conf "$CONFIG_BACKUP"

cleanup() {
    cp "$CONFIG_BACKUP" plugins.conf
    rm -f "$CONFIG_BACKUP"
}

trap cleanup EXIT

echo "========================================"
echo " E-Commerce Checkout Plugin System Demo"
echo "========================================"
echo

echo "[1] Clean and rebuild project"
make clean
make
make bank_transfer
sed -i 's/paypal=enabled/paypal=disabled/' plugins.conf
echo

echo "[2] Show available payment methods from plugins.conf"
./shop_app list
echo

echo "[3] Show payment plugin metadata using dynamic loading"
./shop_app info
echo

echo "[4] Test disabled payment plugin"
./shop_app pay paypal 2000
echo

echo "[5] Enable PayPal plugin by updating plugins.conf"
sed -i 's/paypal=disabled/paypal=enabled/' plugins.conf
cat plugins.conf
echo

echo "[6] Show updated payment methods"
./shop_app list
echo

echo "[7] Process successful direct payments"
./shop_app pay credit_card 1200
echo
./shop_app pay line_pay 800
echo
./shop_app pay paypal 2000
echo

echo "[8] Process failed direct payments based on plugin business rules"
./shop_app pay credit_card 60000
echo
./shop_app pay line_pay 15000
echo
./shop_app pay paypal 35000
echo

echo "[9] E-Commerce checkout with discount and payment plugins"
./shop_app checkout student_discount credit_card
echo
./shop_app checkout festival_discount line_pay
echo
./shop_app checkout no_discount paypal
echo

echo "[10] Show transaction log"
cat transactions.log
echo

echo "[11] Test unknown payment method"
./shop_app pay apple_pay 1000
echo

echo "Demo finished."

