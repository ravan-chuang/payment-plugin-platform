#ifndef DISCOUNT_PLUGIN_H
#define DISCOUNT_PLUGIN_H

typedef struct {
    double discount_amount;
    double final_amount;
    char message[128];
} DiscountResult;

typedef DiscountResult (*ApplyDiscountFunc)(double subtotal);
typedef const char* (*GetDiscountNameFunc)();
typedef const char* (*GetDiscountVersionFunc)();

#endif
