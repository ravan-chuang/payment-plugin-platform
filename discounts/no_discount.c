#include <stdio.h>
#include "../include/discount_plugin.h"

const char* get_discount_name() {
    return "No Discount";
}

const char* get_discount_version() {
    return "1.0.0";
}

DiscountResult apply_discount(double subtotal) {
    DiscountResult result;

    result.discount_amount = 0;
    result.final_amount = subtotal;
    snprintf(result.message, sizeof(result.message), "No discount applied");

    return result;
}

