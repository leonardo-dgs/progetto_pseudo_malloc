#include "intmath.h"

size_t next_pow2(size_t x) {
    if (x == 0)
        return 1;

    size_t power = 1;
    while (power < x)
        power <<= 1;
    return power;
}

size_t ceil_div(size_t a, size_t b) {
    return (a + b - 1) / b;
}

size_t ceil_log2(size_t x) {
    size_t pow = 1;
    size_t result = 0;
    while (pow < x) {
        pow <<= 1;
        result++;
    }
    return result;
}

size_t floor_log2(size_t x) {
    size_t pow = 1;
    size_t result = 0;
    while (pow <= x) {
        pow <<= 1;
        result++;
    }
    return result - 1;
}
