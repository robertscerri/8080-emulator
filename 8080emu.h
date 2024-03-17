#ifndef INC_8080EMULATOR_8080EMU_H
#define INC_8080EMULATOR_8080EMU_H

#include <stdint.h>

typedef struct flags {
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
    uint8_t pad:3;
} flags_t;

typedef struct state_8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct flags flags;
    uint8_t int_enable;
} state_8080_t;

#endif //INC_8080EMULATOR_8080EMU_H
