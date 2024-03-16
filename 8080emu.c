#include <stdio.h>
#include <stdlib.h>
#include "8080emu.h"

void unimplemented_instruction(state_8080_t *state) {
    printf("ERROR: Unimplemented instruction\n");
    exit(1);
}

int emulate_8080(state_8080_t *state) {
    unsigned char *opcode = &state->memory[state->pc];

    switch(*opcode) {
        case 0x00: break;
        case 0x01:
            state->c = opcode[1];
            state->b = opcode[2];
            state->pc += 2;
            break;
        case 0x02:
        {
            uint16_t addr = (state->b << 8) | state->c;
            state->memory[addr] = state->a;
            break;
        }
        case 0x03:
        {
            uint16_t temp = (state->b << 8) | state->c;
            temp++;

            state->b = (temp >> 8) & 0xff;
            state->c = temp & 0xff;
            break;
        }
        case 0x04:
            state->b++;
            state->flags.z = state->b == 0;
            state->flags.s = (state->b >> 7) & 0x01;
            //TODO: Set parity bit
            //TODO: Set aux. carry bit
            break;
        case 0x05:
            state->b--;
            state->flags.z = state->b == 0;
            state->flags.s = (state->b >> 7) & 0x01;
            //TODO: Set parity bit
            //TODO: Set aux. carry bit
            break;
        case 0x06:
            state->b = opcode[1];
            break;
        default:
            unimplemented_instruction(state);
            break;
    }

    state->pc++;
}