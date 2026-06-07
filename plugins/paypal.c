#include <stdio.h>
#include <time.h>
#include "../include/payment_plugin.h"

const char* get_plugin_name() {
    return "PayPal Payment";
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

    if (amount > 30000) {
        result.status = PAYMENT_FAILED;
        snprintf(result.transaction_id, sizeof(result.transaction_id), "N/A");
        snprintf(result.message, sizeof(result.message), "PayPal account verification required");
        return result;
    }

    time_t now = time(NULL);

    result.status = PAYMENT_SUCCESS;
    snprintf(result.transaction_id, sizeof(result.transaction_id), "PP-%ld", now);
    snprintf(result.message, sizeof(result.message),
             "PayPal payment processed successfully");

    return result;
}
