#include <stdio.h>
#include "../include/discount_plugin.h"

const char* get_discount_name() {
    return "Festival Discount";
}

const char* get_discount_version() {
    return "1.0.0";
}

DiscountResult apply_discount(double subtotal) {
    DiscountResult result;

    double discount = subtotal * 0.15;

    if (discount > 500) {
        discount = 500;
    }

    result.discount_amount = discount;
    result.final_amount = subtotal - discount;
    snprintf(result.message, sizeof(result.message),
             "Festival discount 15%% applied, maximum 500");

    return result;
}
