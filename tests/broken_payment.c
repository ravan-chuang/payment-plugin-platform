#include "../include/payment_plugin.h"

const char* get_plugin_name(void) {
    return "Broken Payment Plugin";
}

const char* get_plugin_version(void) {
    return "1.0.0";
}

/*
 * 故意不實作 process_payment()
 * 用來測試 Plugin Validator。
 */
