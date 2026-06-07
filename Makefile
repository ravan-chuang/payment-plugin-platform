CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -ldl

APP = shop_app

PAYMENT_PLUGIN_DIR = build/plugins
DISCOUNT_PLUGIN_DIR = build/discounts

PAYMENT_PLUGINS = credit_card line_pay paypal
DISCOUNT_PLUGINS = no_discount student_discount festival_discount

all: $(APP) payment_plugins discount_plugins

$(APP): src/main.c include/payment_plugin.h include/discount_plugin.h
	$(CC) $(CFLAGS) src/main.c -o $(APP) $(LDFLAGS)

payment_plugins: $(PAYMENT_PLUGINS)

credit_card: plugins/credit_card.c include/payment_plugin.h
	$(CC) -fPIC -shared plugins/credit_card.c -o $(PAYMENT_PLUGIN_DIR)/credit_card.so

line_pay: plugins/line_pay.c include/payment_plugin.h
	$(CC) -fPIC -shared plugins/line_pay.c -o $(PAYMENT_PLUGIN_DIR)/line_pay.so

paypal: plugins/paypal.c include/payment_plugin.h
	$(CC) -fPIC -shared plugins/paypal.c -o $(PAYMENT_PLUGIN_DIR)/paypal.so

bank_transfer: plugins/bank_transfer.c include/payment_plugin.h
	$(CC) -fPIC -shared plugins/bank_transfer.c -o $(PAYMENT_PLUGIN_DIR)/bank_transfer.so

discount_plugins: $(DISCOUNT_PLUGINS)

no_discount: discounts/no_discount.c include/discount_plugin.h
	$(CC) -fPIC -shared discounts/no_discount.c -o $(DISCOUNT_PLUGIN_DIR)/no_discount.so

student_discount: discounts/student_discount.c include/discount_plugin.h
	$(CC) -fPIC -shared discounts/student_discount.c -o $(DISCOUNT_PLUGIN_DIR)/student_discount.so

festival_discount: discounts/festival_discount.c include/discount_plugin.h
	$(CC) -fPIC -shared discounts/festival_discount.c -o $(DISCOUNT_PLUGIN_DIR)/festival_discount.so

clean:
	rm -f $(APP)
	rm -f $(PAYMENT_PLUGIN_DIR)/*.so
	rm -f $(DISCOUNT_PLUGIN_DIR)/*.so
	rm -f transactions.log
