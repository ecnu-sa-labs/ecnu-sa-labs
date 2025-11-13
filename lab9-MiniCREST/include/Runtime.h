void __DSE_Input__(int *x, int ID);

#define DSE_Input(x) __DSE_Input__(&x, __COUNTER__)
