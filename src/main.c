#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include "../include/payment_plugin.h"
#include "../include/discount_plugin.h"

#define CONFIG_FILE "plugins.conf"
#define DISCOUNT_CONFIG_FILE "discounts.conf"
#define MAX_LINE 256
#define MAX_METHOD 64
#define MAX_PATH 256


int find_plugin_path(const char* method, char* plugin_path, int path_size) {
    FILE* file = fopen(CONFIG_FILE, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open config file: %s\n", CONFIG_FILE);
        return -1;
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file) != NULL) {
        char config_method[MAX_METHOD];
        char config_status[32];
        char config_path[MAX_PATH];

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (sscanf(line, "%63[^=]=%31[^=]=%255s",
                   config_method, config_status, config_path) == 3) {
            if (strcmp(method, config_method) == 0) {
                if (strcmp(config_status, "enabled") != 0) {
                    fclose(file);
                    return 0;
                }

                snprintf(plugin_path, path_size, "%s", config_path);
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return -1;
}

int find_discount_path(const char* method, char* plugin_path, int path_size) {
    FILE* file = fopen(DISCOUNT_CONFIG_FILE, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open config file: %s\n", DISCOUNT_CONFIG_FILE);
        return -1;
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file) != NULL) {
        char config_method[MAX_METHOD];
        char config_status[32];
        char config_path[MAX_PATH];

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (sscanf(line, "%63[^=]=%31[^=]=%255s",
                   config_method, config_status, config_path) == 3) {
            if (strcmp(method, config_method) == 0) {
                if (strcmp(config_status, "enabled") != 0) {
                    fclose(file);
                    return 0;
                }

                snprintf(plugin_path, path_size, "%s", config_path);
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return -1;
}

void list_payment_methods() {
    FILE* file = fopen(CONFIG_FILE, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open config file: %s\n", CONFIG_FILE);
        return;
    }

    printf("Available Payment Methods:\n");

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file) != NULL) {
        char config_method[MAX_METHOD];
        char config_status[32];
        char config_path[MAX_PATH];

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (sscanf(line, "%63[^=]=%31[^=]=%255s",
                   config_method, config_status, config_path) == 3) {
            if (strcmp(config_status, "enabled") == 0) {
                printf("- %s\n", config_method);
            }
        }
    }

    fclose(file);
}

void show_plugin_info() {
    FILE* file = fopen(CONFIG_FILE, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open config file: %s\n", CONFIG_FILE);
        return;
    }

    printf("Registered Payment Plugins:\n\n");

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file) != NULL) {
        char config_method[MAX_METHOD];
        char config_status[32];
        char config_path[MAX_PATH];

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (sscanf(line, "%63[^=]=%31[^=]=%255s",
                   config_method, config_status, config_path) == 3) {
            void* handle = dlopen(config_path, RTLD_NOW);

            if (!handle) {
                printf("- method: %s\n", config_method);
                printf("  status: %s\n", config_status);
                printf("  load: FAILED\n");
                printf("  path: %s\n", config_path);
                printf("  error: %s\n\n", dlerror());
                continue;
            }

            dlerror();

            GetPluginNameFunc get_plugin_name =
                (GetPluginNameFunc)dlsym(handle, "get_plugin_name");

            GetPluginVersionFunc get_plugin_version =
                (GetPluginVersionFunc)dlsym(handle, "get_plugin_version");

            char* error = dlerror();

            if (error != NULL) {
                printf("- method: %s\n", config_method);
                printf("  status: %s\n", config_status);
                printf("  load: INVALID PLUGIN INTERFACE\n");
                printf("  path: %s\n", config_path);
                printf("  error: %s\n\n", error);
                dlclose(handle);
                continue;
            }

            printf("- method: %s\n", config_method);
            printf("  status: %s\n", config_status);
            printf("  name: %s\n", get_plugin_name());
            printf("  version: %s\n", get_plugin_version());
            printf("  path: %s\n\n", config_path);

            dlclose(handle);
        }
    }

    fclose(file);
}

void write_transaction_log(const char* method, double amount, PaymentResult result) {
    FILE* log_file = fopen("transactions.log", "a");

    if (log_file == NULL) {
        fprintf(stderr, "Warning: cannot open transactions.log\n");
        return;
    }

    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);

    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    fprintf(log_file,
            "%s | method=%s | amount=%.2f | status=%s | tx_id=%s | message=%s\n",
            time_str,
            method,
            amount,
            result.status == PAYMENT_SUCCESS ? "SUCCESS" : "FAILED",
            result.transaction_id,
            result.message);

    fclose(log_file);
}

void checkout_order(const char* discount_method, const char* payment_method) {
    double mouse_price = 599.0;
    double keyboard_price = 1290.0;
    double cable_price = 199.0;
    int cable_qty = 2;

    double subtotal = mouse_price + keyboard_price + cable_price * cable_qty;

    printf("=== E-Commerce Checkout System ===\n\n");
    printf("Cart Items:\n");
    printf("- Wireless Mouse x1: %.2f\n", mouse_price);
    printf("- Mechanical Keyboard x1: %.2f\n", keyboard_price);
    printf("- USB-C Cable x%d: %.2f\n", cable_qty, cable_price * cable_qty);
    printf("\nSubtotal: %.2f\n\n", subtotal);

    char discount_path[MAX_PATH];
    int discount_status = find_discount_path(discount_method, discount_path, sizeof(discount_path));

    if (discount_status == -1) {
        fprintf(stderr, "Error: discount method not found in %s\n", DISCOUNT_CONFIG_FILE);
        fprintf(stderr, "Method: %s\n", discount_method);
        return;
    }

    if (discount_status == 0) {
        fprintf(stderr, "Error: discount method is disabled\n");
        fprintf(stderr, "Method: %s\n", discount_method);
        return;
    }

    printf("Loading Discount Plugin: %s\n", discount_path);

    void* discount_handle = dlopen(discount_path, RTLD_NOW);

    if (!discount_handle) {
        fprintf(stderr, "Error: failed to load discount plugin\n");
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return;
    }

    dlerror();

    GetDiscountNameFunc get_discount_name =
        (GetDiscountNameFunc)dlsym(discount_handle, "get_discount_name");

    GetDiscountVersionFunc get_discount_version =
        (GetDiscountVersionFunc)dlsym(discount_handle, "get_discount_version");

    ApplyDiscountFunc apply_discount =
        (ApplyDiscountFunc)dlsym(discount_handle, "apply_discount");

    char* discount_error = dlerror();

    if (discount_error != NULL) {
        fprintf(stderr, "Error: failed to find required discount symbol\n");
        fprintf(stderr, "dlsym error: %s\n", discount_error);
        dlclose(discount_handle);
        return;
    }

    printf("Discount Name: %s\n", get_discount_name());
    printf("Discount Version: %s\n", get_discount_version());

    DiscountResult discount_result = apply_discount(subtotal);

    printf("Discount Amount: %.2f\n", discount_result.discount_amount);
    printf("Final Amount: %.2f\n", discount_result.final_amount);
    printf("Discount Message: %s\n\n", discount_result.message);

    dlclose(discount_handle);

    char payment_path[MAX_PATH];
    int payment_status = find_plugin_path(payment_method, payment_path, sizeof(payment_path));

    if (payment_status == -1) {
        fprintf(stderr, "Error: payment method not found in %s\n", CONFIG_FILE);
        fprintf(stderr, "Method: %s\n", payment_method);
        return;
    }

    if (payment_status == 0) {
        fprintf(stderr, "Error: payment method is disabled\n");
        fprintf(stderr, "Method: %s\n", payment_method);
        return;
    }

    printf("Loading Payment Plugin: %s\n", payment_path);

    void* payment_handle = dlopen(payment_path, RTLD_NOW);

    if (!payment_handle) {
        fprintf(stderr, "Error: failed to load payment plugin\n");
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return;
    }

    dlerror();

    GetPluginNameFunc get_plugin_name =
        (GetPluginNameFunc)dlsym(payment_handle, "get_plugin_name");

    GetPluginVersionFunc get_plugin_version =
        (GetPluginVersionFunc)dlsym(payment_handle, "get_plugin_version");

    ProcessPaymentFunc process_payment =
        (ProcessPaymentFunc)dlsym(payment_handle, "process_payment");

    char* payment_error = dlerror();

    if (payment_error != NULL) {
        fprintf(stderr, "Error: failed to find required payment symbol\n");
        fprintf(stderr, "dlsym error: %s\n", payment_error);
        dlclose(payment_handle);
        return;
    }

    printf("Payment Name: %s\n", get_plugin_name());
    printf("Payment Version: %s\n", get_plugin_version());
    printf("Processing final amount: %.2f\n\n", discount_result.final_amount);

    PaymentResult payment_result = process_payment(discount_result.final_amount);

    if (payment_result.status == PAYMENT_SUCCESS) {
        printf("Payment Status: SUCCESS\n");
    } else {
        printf("Payment Status: FAILED\n");
    }

    printf("Transaction ID: %s\n", payment_result.transaction_id);
    printf("Message: %s\n", payment_result.message);

    write_transaction_log(payment_method, discount_result.final_amount, payment_result);
    printf("\nCheckout transaction saved to transactions.log\n");

    dlclose(payment_handle);
}

static void validate_plugin_config(
    const char* category_name,
    const char* config_file,
    const char* required_symbols[],
    int symbol_count,
    int* passed_count,
    int* failed_count
) {
    FILE* file = fopen(config_file, "r");

    if (file == NULL) {
        fprintf(stderr, "\n[ERROR] Cannot open config file: %s\n", config_file);
        (*failed_count)++;
        return;
    }

    printf("\n%s\n", category_name);
    printf("----------------------------------------\n");

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file) != NULL) {
        char method[MAX_METHOD];
        char status[32];
        char plugin_path[MAX_PATH];

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (sscanf(
                line,
                "%63[^=]=%31[^=]=%255s",
                method,
                status,
                plugin_path
            ) != 3) {
            printf("\n[FAIL] Invalid configuration entry\n");
            printf("  entry: %s\n", line);

            (*failed_count)++;
            continue;
        }

        printf("\nPlugin: %s\n", method);
        printf("  status: %s\n", status);
        printf("  path: %s\n", plugin_path);

        void* handle = dlopen(plugin_path, RTLD_NOW);

        if (handle == NULL) {
            printf("  dlopen: FAILED\n");
            printf("  error: %s\n", dlerror());
            printf("  result: FAIL\n");

            (*failed_count)++;
            continue;
        }

        printf("  dlopen: success\n");

        int plugin_valid = 1;

        for (int i = 0; i < symbol_count; i++) {
            dlerror();

            void* symbol = dlsym(handle, required_symbols[i]);
            const char* error = dlerror();

            if (error != NULL || symbol == NULL) {
                printf("  %-24s: MISSING\n", required_symbols[i]);
                plugin_valid = 0;
            } else {
                printf("  %-24s: found\n", required_symbols[i]);
            }
        }

        dlclose(handle);

        if (plugin_valid) {
            printf("  result: PASS\n");
            (*passed_count)++;
        } else {
            printf("  result: FAIL\n");
            (*failed_count)++;
        }
    }

    fclose(file);
}

void validate_all_plugins(void) {
    const char* payment_symbols[] = {
        "get_plugin_name",
        "get_plugin_version",
        "process_payment"
    };

    const char* discount_symbols[] = {
        "get_discount_name",
        "get_discount_version",
        "apply_discount"
    };

    int passed_count = 0;
    int failed_count = 0;

    printf("========================================\n");
    printf(" Plugin Validation Report\n");
    printf("========================================\n");

    validate_plugin_config(
        "Payment Plugins",
        CONFIG_FILE,
        payment_symbols,
        3,
        &passed_count,
        &failed_count
    );

    validate_plugin_config(
        "Discount Plugins",
        DISCOUNT_CONFIG_FILE,
        discount_symbols,
        3,
        &passed_count,
        &failed_count
    );

    printf("\n========================================\n");
    printf(" Validation Summary\n");
    printf("========================================\n");
    printf("Passed: %d\n", passed_count);
    printf("Failed: %d\n", failed_count);

    if (failed_count == 0) {
        printf("Overall Result: ALL PLUGINS VALID\n");
    } else {
        printf("Overall Result: VALIDATION FAILED\n");
    }
}

int main(int argc, char* argv[]) {

    if (argc == 2 && strcmp(argv[1], "list") == 0) {
        list_payment_methods();
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "info") == 0) {
        show_plugin_info();
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "validate") == 0) {
        validate_all_plugins();
        return 0;
    }

    if (argc == 4 && strcmp(argv[1], "checkout") == 0) {
        checkout_order(argv[2], argv[3]);
        return 0;
    }

    if (argc != 4 || strcmp(argv[1], "pay") != 0) {
        printf("Usage:\n");
        printf("  %s list\n", argv[0]);
        printf("  %s info\n", argv[0]);
        printf("  %s validate\n", argv[0]);
        printf("  %s pay <payment_method> <amount>\n", argv[0]);
        printf("  %s checkout <discount_method> <payment_method>\n", argv[0]);
        printf("\nExamples:\n");
        printf("  %s list\n", argv[0]);
        printf("  %s info\n", argv[0]);
        printf("  %s validate\n", argv[0]);
        printf("  %s pay credit_card 1200\n", argv[0]);
        printf("  %s pay line_pay 800\n", argv[0]);
        printf("  %s checkout student_discount credit_card\n", argv[0]);
        printf("  %s checkout festival_discount line_pay\n", argv[0]);
        return 1;
    }

    char* method = argv[2];
    double amount = atof(argv[3]);

    if (amount <= 0) {
        fprintf(stderr, "Error: amount must be greater than 0\n");
        return 1;
    }

    char plugin_path[MAX_PATH];

    int plugin_status = find_plugin_path(method, plugin_path, sizeof(plugin_path));

    if (plugin_status == -1) {
        fprintf(stderr, "Error: payment method not found in %s\n", CONFIG_FILE);
        fprintf(stderr, "Method: %s\n", method);
        return 1;
    }

    if (plugin_status == 0) {
        fprintf(stderr, "Error: payment method is disabled\n");
        fprintf(stderr, "Method: %s\n", method);
        return 1;
    }

    printf("=== Commercial Payment Plugin Platform ===\n");
    printf("Selected Method: %s\n", method);
    printf("Amount: %.2f\n", amount);
    printf("Loading Plugin: %s\n\n", plugin_path);

    void* handle = dlopen(plugin_path, RTLD_NOW);

    if (!handle) {
        fprintf(stderr, "Error: failed to load plugin\n");
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return 1;
    }

    dlerror();

    GetPluginNameFunc get_plugin_name =
        (GetPluginNameFunc)dlsym(handle, "get_plugin_name");

    GetPluginVersionFunc get_plugin_version =
        (GetPluginVersionFunc)dlsym(handle, "get_plugin_version");

    ProcessPaymentFunc process_payment =
        (ProcessPaymentFunc)dlsym(handle, "process_payment");

    char* error = dlerror();

    if (error != NULL) {
        fprintf(stderr, "Error: failed to find required symbol\n");
        fprintf(stderr, "dlsym error: %s\n", error);
        dlclose(handle);
        return 1;
    }

    printf("Plugin Name: %s\n", get_plugin_name());
    printf("Plugin Version: %s\n", get_plugin_version());
    printf("Processing payment...\n\n");

    PaymentResult result = process_payment(amount);

    if (result.status == PAYMENT_SUCCESS) {
        printf("Payment Status: SUCCESS\n");
    } else {
        printf("Payment Status: FAILED\n");
    }

    printf("Transaction ID: %s\n", result.transaction_id);
    printf("Message: %s\n", result.message);

    write_transaction_log(method, amount, result);
    printf("\nTransaction saved to transactions.log\n");

    dlclose(handle);

    return 0;
}
