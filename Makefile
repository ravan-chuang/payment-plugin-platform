CC := gcc
CFLAGS := -Wall -Wextra -Iinclude
PLUGIN_FLAGS := -Wall -Wextra -fPIC -shared -Iinclude
LDLIBS := -ldl

APP := shop_app

PAYMENT_PLUGIN_DIR := build/plugins
DISCOUNT_PLUGIN_DIR := build/discounts

PAYMENT_PLUGINS := credit_card line_pay paypal bank_transfer
DISCOUNT_PLUGINS := no_discount student_discount festival_discount

PAYMENT_SO := $(addprefix $(PAYMENT_PLUGIN_DIR)/,$(addsuffix .so,$(PAYMENT_PLUGINS)))
DISCOUNT_SO := $(addprefix $(DISCOUNT_PLUGIN_DIR)/,$(addsuffix .so,$(DISCOUNT_PLUGINS)))

.RECIPEPREFIX := >

.PHONY: all prepare payment_plugins discount_plugins clean

all: prepare $(APP) payment_plugins discount_plugins

prepare:
>mkdir -p $(PAYMENT_PLUGIN_DIR) $(DISCOUNT_PLUGIN_DIR)

$(APP): src/main.c include/payment_plugin.h include/discount_plugin.h
>$(CC) $(CFLAGS) src/main.c -o $(APP) $(LDLIBS)

payment_plugins: $(PAYMENT_SO)

discount_plugins: $(DISCOUNT_SO)

$(PAYMENT_PLUGIN_DIR)/%.so: plugins/%.c include/payment_plugin.h | prepare
>$(CC) $(PLUGIN_FLAGS) $< -o $@

$(DISCOUNT_PLUGIN_DIR)/%.so: discounts/%.c include/discount_plugin.h | prepare
>$(CC) $(PLUGIN_FLAGS) $< -o $@

clean:
>rm -f $(APP)
>rm -rf build
>rm -f transactions.log
