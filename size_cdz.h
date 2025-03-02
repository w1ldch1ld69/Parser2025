#pragma once

#define DEFAULT_TARRAY_SIZE 32
#define DEFAULT_ARG_SIZE 4

typedef unsigned long size_cdz;

typedef struct {
    const char *name;
    double value;
} variable;

const variable constants[] = {
    {"e", 2.7182}, {"pi", 3.1415}
};

typedef struct func {
    char *name;
    size_cdz argCntr;
    double (*f)(double *);
} function;

typedef struct ders {
    char *func;
    char *der;
} ders;

int
isChar(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))
        return 1;
    else
        return 0;
}

int
isDigit(char c) {
    if (c >= '0' && c <= '9')
        return 1;
    else 
        return 0;
}

int
fact(int n) {
    return (n == 0) ? 1 : n * fact(n - 1);
}

double
int_pow(double base, int power) {
    double x = base;
    for (int i = 1; i < power; ++i) 
        x *= base;
    return x;
} 

double
sin(double* args) {
    double x = args[0];
    return x - int_pow(x, 3) / fact(3) + int_pow(x, 5) / fact(5) - int_pow(x, 7) / fact(7); 
}

double
cos(double* args) {
    double x = args[0];
    return 1 - int_pow(x, 2) / fact(2) + int_pow(x, 4) / fact(4) - int_pow(x, 6) / fact(6);
}

double
tg(double *args) {
    return sin(args) / cos(args);
}

double
ctg(double *args) {
    return cos(args) / sin(args);
}

double
pow(double *args) {
    double x = args[0] - 1;
    double alpha = args[1];
    return 1 + alpha * x + alpha * (alpha - 1) * int_pow(x, 2) / fact(2) + alpha * (alpha - 1) * (alpha - 2) * int_pow(x, 3) / fact(3) + alpha * (alpha - 1) * (alpha - 2) * (alpha - 3) * int_pow(x, 3) / fact(4); 
}

double
log(double *args) {
    double x = args[0] - 1;
    return x - int_pow(x, 2) / 2 + int_pow(x, 3) / 3 - int_pow(x, 4) / 4 + int_pow(x, 5) / 5;
}

double
abss(double *args) {
    return (args[0] >= 0) ? args[0] : -args[0];
}

const function systemFuncs[] = {
    {"sin", 1, sin}, {"cos", 1, cos}, {"tg", 1, tg}, {"ctg", 1, ctg},
    {"log", 1, log}, {"abss", 1, abss}, {"pow", 2, pow}
};

const ders derFuncs[] = {
    {"sin", "-cos"}, {"cos", "-sin"}, {"log", "1/"}
};