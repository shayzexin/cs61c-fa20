#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
    bool bit_0 = (*reg >> 0) & 1;
    bool bit_2 = (*reg >> 2) & 1;
    bool bit_3 = (*reg >> 3) & 1;
    bool bit_5 = (*reg >> 5) & 1;
    
    bool new_bit = bit_0 ^ bit_2 ^ bit_3 ^ bit_5;
    
    *reg = (*reg >> 1) | (new_bit << 15);
}

