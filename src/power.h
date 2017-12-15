#define powermgmt_callback void

void power_register_init_callback(powermgmt_callback *c);
void power_register_sleep_callback(powermgmt_callback *c);
void power_register_wakeup_callback(powermgmt_callback *c);

void power_init();
void power_sleep();
void power_wakeup();
