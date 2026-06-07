#include <stdio.h>
#include <time.h>
#include "../include/payment_plugin.h"

const char* get_plugin_name() {
    return "Bank Transfer Payment";
}

const char* get_plugin_version() {
    return "1.0.0";
}

PaymentResult process_payment(double amount) {
    PaymentResult result;

    if (amount > 100000.0) {
        result.status = PAYMENT_FAILED;

        snprintf(
            result.transaction_id,
            sizeof(result.transaction_id),
            "N/A"
        );

        snprintf(
            result.message,
            sizeof(result.message),
            "Bank transfer amount exceeds the verification limit"
        );

        return result;
    }

    result.status = PAYMENT_SUCCESS;

    snprintf(
        result.transaction_id,
        sizeof(result.transaction_id),
        "BT-%ld",
        time(NULL)
    );

    snprintf(
        result.message,
        sizeof(result.message),
        "Bank transfer payment processed successfully"
    );

    return result;
}
