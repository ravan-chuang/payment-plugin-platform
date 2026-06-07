#include <stdio.h>
#include <time.h>
#include "../include/payment_plugin.h"

const char* get_plugin_name() {
    return "Credit Card Payment";
}

const char* get_plugin_version() {
    return "1.0.0";
}

PaymentResult process_payment(double amount) {
    PaymentResult result;

    if (amount <= 0) {
        result.status = PAYMENT_FAILED;
        snprintf(result.transaction_id, sizeof(result.transaction_id), "N/A");
        snprintf(result.message, sizeof(result.message), "Invalid payment amount");
        return result;
    }

    if (amount > 50000) {
        result.status = PAYMENT_FAILED;
        snprintf(result.transaction_id, sizeof(result.transaction_id), "N/A");
        snprintf(result.message, sizeof(result.message), "Credit card limit exceeded");
        return result;
    }

    time_t now = time(NULL);

    result.status = PAYMENT_SUCCESS;
    snprintf(result.transaction_id, sizeof(result.transaction_id), "CC-%ld", now);
    snprintf(result.message, sizeof(result.message),
             "Credit card payment processed successfully");

    return result;
}
