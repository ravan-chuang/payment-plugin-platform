#include <stdio.h>
#include "../include/discount_plugin.h"

const char* get_discount_name() {
    return "Student Discount";
}

const char* get_discount_version() {
    return "1.0.0";
}

DiscountResult apply_discount(double subtotal) {
    DiscountResult result;

    double discount = 200.0;

    if (subtotal < 1000) {
        discount = 100.0;
    }

    if (discount > subtotal) {
        discount = subtotal;
    }

    result.discount_amount = discount;
    result.final_amount = subtotal - discount;
    snprintf(result.message, sizeof(result.message),
             "Student discount applied");

    return result;
}
