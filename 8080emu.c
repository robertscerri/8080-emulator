#include <stdio.h>
#include <stdlib.h>
#include "8080emu.h"

void unimplemented_instruction(state_8080_t *state, unsigned char opcode);
uint16_t get_2byte_word(uint8_t msb, uint8_t lsb);

void unimplemented_instruction(state_8080_t *state, unsigned char opcode) {
    printf("ERROR: Unimplemented instruction (0x%02x)\n", opcode);
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
                uint16_t addr = get_2byte_word(state->b, state->c);
                state->memory[addr] = state->a;
                break;
            }
        case 0x03:
            {
                uint16_t temp = get_2byte_word(state->b, state->c);
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
        case 0x07:
            state->flags.cy = (state->a >> 7) & 0x01;
            state->a = ((state->a << 1) && 0xff) + state->flags.cy;
            break;
        //TODO: Implement opcodes 0x08-0x3f
        case 0x40:
            state->b = state->b;
            break;
        case 0x41:
            state->b = state->c;
            break;
        case 0x42:
            state->b = state->d;
            break;
        case 0x43:
            state->b = state->e;
            break;
        case 0x44:
            state->b = state->h;
            break;
        case 0x45:
            state->b = state->l;
            break;
        case 0x46:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->b = state->memory[addr];
                break;
            }
        case 0x47:
            state->b = state->a;
            break;
        case 0x48:
            state->c = state->b;
            break;
        case 0x49:
            state->c = state->c;
            break;
        case 0x4a:
            state->c = state->d;
            break;
        case 0x4b:
            state->c = state->e;
            break;
        case 0x4c:
            state->c = state->h;
            break;
        case 0x4d:
            state->c = state->l;
            break;
        case 0x4e:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->c = state->memory[addr];
                break;
            }
        case 0x4f:
            state->c = state->a;
            break;
        case 0x50:
            state->d = state->b;
            break;
        case 0x51:
            state->d = state->c;
            break;
        case 0x52:
            state->d = state->d;
            break;
        case 0x53:
            state->d = state->e;
            break;
        case 0x54:
            state->d = state->h;
            break;
        case 0x55:
            state->d = state->l;
            break;
        case 0x56:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->d = state->memory[addr];
                break;
            }
        case 0x57:
            state->d = state->a;
            break;
        case 0x58:
            state->e = state->b;
            break;
        case 0x59:
            state->e = state->c;
            break;
        case 0x5a:
            state->e = state->d;
            break;
        case 0x5b:
            state->e = state->e;
            break;
        case 0x5c:
            state->e = state->h;
            break;
        case 0x5d:
            state->e = state->l;
            break;
        case 0x5e:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->e = state->memory[addr];
                break;
            }
        case 0x5f:
            state->e = state->a;
            break;
        case 0x60:
            state->h = state->b;
            break;
        case 0x61:
            state->h = state->c;
            break;
        case 0x62:
            state->h = state->d;
            break;
        case 0x63:
            state->h = state->e;
            break;
        case 0x64:
            state->h = state->h;
            break;
        case 0x65:
            state->h = state->l;
            break;
        case 0x66:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->h = state->memory[addr];
                break;
            }
        case 0x67:
            state->h = state->a;
            break;
        case 0x68:
            state->l = state->b;
            break;
        case 0x69:
            state->l = state->c;
            break;
        case 0x6a:
            state->l = state->d;
            break;
        case 0x6b:
            state->l = state->e;
            break;
        case 0x6c:
            state->l = state->h;
            break;
        case 0x6d:
            state->l = state->l;
            break;
        case 0x6e:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->l = state->memory[addr];
                break;
            }
        case 0x6f:
            state->l = state->a;
            break;
        case 0x70:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->memory[addr] = state->b;
                break;
            }
        case 0x71:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->memory[addr] = state->c;
                break;
            }
        case 0x72:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->memory[addr] = state->d;
                break;
            }
        case 0x73:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->memory[addr] = state->e;
                break;
            }
        case 0x74:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->memory[addr] = state->h;
                break;
            }
        case 0x75:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->memory[addr] = state->l;
                break;
            }
        case 0x76:
            //TODO: Implement HLT instruction
            break;
        case 0x77:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->memory[addr] = state->a;
                break;
            }
        case 0x78:
            state->a = state->b;
            break;
        case 0x79:
            state->a = state->c;
            break;
        case 0x7a:
            state->a = state->d;
            break;
        case 0x7b:
            state->a = state->e;
            break;
        case 0x7c:
            state->a = state->h;
            break;
        case 0x7d:
            state->a = state->l;
            break;
        case 0x7e:
            {
                int addr = get_2byte_word(state->h, state->l);
                state->a = state->memory[addr];
                break;
            }
        case 0x7f:
            state->a = state->a;
            break;
        default:
            unimplemented_instruction(state, *opcode);
            break;
    }

    state->pc++;
}

uint16_t get_2byte_word(uint8_t msb, uint8_t lsb) {
    return (msb << 8) | lsb;
}