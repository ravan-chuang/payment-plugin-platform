#ifndef PAYMENT_PLUGIN_H
#define PAYMENT_PLUGIN_H

#define PAYMENT_SUCCESS 1
#define PAYMENT_FAILED 0

typedef struct {
    int status;
    char transaction_id[64];
    char message[128];
} PaymentResult;

typedef PaymentResult (*ProcessPaymentFunc)(double amount);
typedef const char* (*GetPluginNameFunc)();
typedef const char* (*GetPluginVersionFunc)();

#endif
