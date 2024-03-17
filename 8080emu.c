#include <stdio.h>
#include <stdlib.h>
#include "8080emu.h"

void unimplemented_instruction(state_8080_t *state, unsigned char opcode);

uint16_t get_2byte_word(uint8_t msb, uint8_t lsb);

//Flags
int get_sign_bit(uint8_t num);
int get_parity_bit(uint8_t num);

void perform_stax(state_8080_t *state, const uint8_t msb, const uint8_t lsb);
void perform_ldax(state_8080_t *state, const uint8_t msb, const uint8_t lsb);

//Arithmetic Group
void perform_inr(state_8080_t *state, const uint8_t *reg);
void perform_dcr(state_8080_t *state, const uint8_t *reg);
void perform_inx(state_8080_t *state, uint8_t *msb, uint8_t *lsb);
void perform_dcx(state_8080_t *state, uint8_t *msb, uint8_t *lsb);
void perform_add(state_8080_t *state, const uint8_t operand);
void perform_adc(state_8080_t *state, const uint8_t operand);
void perform_sub(state_8080_t *state, const uint8_t operand);
void perform_sbb(state_8080_t *state, const uint8_t operand);

//Logical Group
void perform_ana(state_8080_t *state, const uint8_t operand);
void perform_xra(state_8080_t *state, const uint8_t operand);
void perform_ora(state_8080_t *state, const uint8_t operand);
void perform_cmp(state_8080_t *state, const uint8_t operand);

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
            perform_stax(state, state->b, state->c);
            break;
        case 0x03:
            perform_inx(state, &state->b, &state->c);
            break;
        case 0x04:
            perform_inr(state, &state->b);
            break;
        case 0x05:
            perform_dcr(state, &state->b);
            break;
        case 0x06:
            state->b = opcode[1];
            state->pc += 1;
            break;
        case 0x07:
            {
                uint8_t prevBit7 = state->a & 0x80;
                state->a = state->a << 1;
                state->a = state->a | prevBit7;
                state->flags.cy = prevBit7;
                break;
            }
        //TODO: Implement opcode 0x09
        case 0x0a:
            perform_ldax(state, state->b, state->c);
            break;
        case 0x0b:
            perform_dcx(state, &state->b, &state->c);
            break;
        case 0x0c:
            perform_inr(state, &state->c);
            break;
        case 0x0d:
            perform_dcr(state, &state->c);
            break;
        case 0x0e:
            state->c = opcode[1];
            state->pc += 1;
            break;
        case 0x0f:
            {
                uint8_t prevBit0 = state->a & 0x01;
                state->a = state->a >> 1;
                state->a = state->a | (prevBit0 << 7);
                state->flags.cy = prevBit0;
                break;
            }
        case 0x11:
            state->e = opcode[1];
            state->d = opcode[2];
            state->pc += 2;
            break;
        case 0x12:
            perform_stax(state, state->d, state->e);
            break;
        case 0x13:
            perform_inx(state, &state->d, &state->e);
            break;
        case 0x14:
            perform_inr(state, &state->d);
            break;
        case 0x15:
            perform_dcr(state, &state->d);
            break;
        case 0x16:
            state->d = opcode[1];
            state->pc += 1;
            break;
        case 0x17:
            {
                uint8_t prevBit7 = state->a & 0x80;
                state->a = state->a << 1;
                state->a = state->a | (state->flags.cy & 0x01);
                state->flags.cy = prevBit7;
                break;
            }
        //TODO: Implement opcode 0x19
        case 0x1a:
            perform_ldax(state, state->d, state->e);
            break;
        case 0x1b:
            perform_dcx(state, &state->d, &state->e);
            break;
        case 0x1c:
            perform_inr(state, &state->e);
            break;
        case 0x1d:
            perform_dcr(state, &state->e);
            break;
        case 0x1e:
            state->e = opcode[1];
            state->pc += 1;
            break;
        //TODO: Implement opcodes 0x1f -> 0x3f
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
        case 0x80:
            perform_add(state, state->b);
            break;
        case 0x81:
            perform_add(state, state->c);
            break;
        case 0x82:
            perform_add(state, state->d);
            break;
        case 0x83:
            perform_add(state, state->e);
            break;
        case 0x84:
            perform_add(state, state->h);
            break;
        case 0x85:
            perform_add(state, state->l);
            break;
        case 0x86:
            perform_add(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0x87:
            perform_add(state, state->a);
            break;
        case 0x88:
            perform_adc(state, state->b);
            break;
        case 0x89:
            perform_adc(state, state->c);
            break;
        case 0x8a:
            perform_adc(state, state->d);
            break;
        case 0x8b:
            perform_adc(state, state->e);
            break;
        case 0x8c:
            perform_adc(state, state->h);
            break;
        case 0x8d:
            perform_adc(state, state->l);
            break;
        case 0x8e:
            perform_adc(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0x8f:
            perform_adc(state, state->a);
            break;
        case 0x90:
            perform_sub(state, state->b);
            break;
        case 0x91:
            perform_sub(state, state->c);
            break;
        case 0x92:
            perform_sub(state, state->d);
            break;
        case 0x93:
            perform_sub(state, state->e);
            break;
        case 0x94:
            perform_sub(state, state->h);
            break;
        case 0x95:
            perform_sub(state, state->l);
            break;
        case 0x96:
            perform_sub(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0x97:
            perform_sub(state, state->a);
            break;
        case 0x98:
            perform_sbb(state, state->b);
            break;
        case 0x99:
            perform_sbb(state, state->c);
            break;
        case 0x9a:
            perform_sbb(state, state->d);
            break;
        case 0x9b:
            perform_sbb(state, state->e);
            break;
        case 0x9c:
            perform_sbb(state, state->h);
            break;
        case 0x9d:
            perform_sbb(state, state->l);
            break;
        case 0x9e:
            perform_sbb(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0x9f:
            perform_sbb(state, state->a);
            break;
        case 0xa0:
            perform_ana(state, state->b);
            break;
        case 0xa1:
            perform_ana(state, state->c);
            break;
        case 0xa2:
            perform_ana(state, state->d);
            break;
        case 0xa3:
            perform_ana(state, state->e);
            break;
        case 0xa4:
            perform_ana(state, state->h);
            break;
        case 0xa5:
            perform_ana(state, state->l);
            break;
        case 0xa6:
            perform_ana(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0xa7:
            perform_ana(state, state->a);
            break;
        case 0xa8:
            perform_xra(state, state->b);
            break;
        case 0xa9:
            perform_xra(state, state->c);
            break;
        case 0xaa:
            perform_xra(state, state->d);
            break;
        case 0xab:
            perform_xra(state, state->e);
            break;
        case 0xac:
            perform_xra(state, state->h);
            break;
        case 0xad:
            perform_xra(state, state->l);
            break;
        case 0xae:
            perform_xra(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0xaf:
            perform_xra(state, state->a);
            break;
        case 0xb0:
            perform_ora(state, state->b);
            break;
        case 0xb1:
            perform_ora(state, state->c);
            break;
        case 0xb2:
            perform_ora(state, state->d);
            break;
        case 0xb3:
            perform_ora(state, state->e);
            break;
        case 0xb4:
            perform_ora(state, state->h);
            break;
        case 0xb5:
            perform_ora(state, state->l);
            break;
        case 0xb6:
            perform_ora(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0xb7:
            perform_ora(state, state->a);
            break;
        case 0xb8:
            perform_cmp(state, state->b);
            break;
        case 0xb9:
            perform_cmp(state, state->c);
            break;
        case 0xba:
            perform_cmp(state, state->d);
            break;
        case 0xbb:
            perform_cmp(state, state->e);
            break;
        case 0xbc:
            perform_cmp(state, state->h);
            break;
        case 0xbd:
            perform_cmp(state, state->l);
            break;
        case 0xbe:
            perform_cmp(state, state->memory[get_2byte_word(state->h, state->l)]);
            break;
        case 0xbf:
            perform_cmp(state, state->a);
            break;
        //TODO: Implement opcodes 0xc0 -> 0xff
        default:
            unimplemented_instruction(state, *opcode);
            break;
    }

    state->pc++;
}

uint16_t get_2byte_word(uint8_t msb, uint8_t lsb) {
    return (msb << 8) | lsb;
}

int get_sign_bit(uint8_t num) {
    return (num & 0x80) != 0 ? 1 : 0;
}

int get_parity_bit(uint8_t num) {
    int parity = 1;

    while (num) {
        parity ^= (num & 1);
        num >>= 1;
    }

    return parity;
}

void perform_stax(state_8080_t *state, const uint8_t msb, const uint8_t lsb) {
    uint16_t addr = get_2byte_word(msb, lsb);
    state->memory[addr] = state->a;
}

void perform_ldax(state_8080_t *state, const uint8_t msb, const uint8_t lsb) {
    uint16_t addr = get_2byte_word(msb, lsb);
    state->a = state->memory[addr];
}

void perform_inr(state_8080_t *state, const uint8_t *reg) {
    *reg++;

    state->flags.z = *reg == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(*reg);
    state->flags.p = get_parity_bit(*reg);
    //TODO: Add auxiliary carry bit
}

void perform_dcr(state_8080_t *state, const uint8_t *reg) {
    *reg--;

    state->flags.z = *reg == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(*reg);
    state->flags.p = get_parity_bit(*reg);
    //TODO: Add auxiliary carry bit
}

void perform_inx(state_8080_t *state, uint8_t *msb, uint8_t *lsb) {
    uint16_t result = get_2byte_word(state->b, state->c);
    result++;

    *msb = (result >> 8) & 0xff;
    *lsb = result & 0xff;
}

void perform_dcx(state_8080_t *state, uint8_t *msb, uint8_t *lsb) {
    uint16_t result = get_2byte_word(state->b, state->c);
    result--;

    *msb = (result >> 8) & 0xff;
    *lsb = result & 0xff;
}

void perform_add(state_8080_t *state, const uint8_t operand) {
    uint16_t result = (uint16_t) state->a + (uint16_t) operand;
    state->a = (uint8_t) (result & 0xff);

    state->flags.z = state->a == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(state->a);
    state->flags.p = get_parity_bit(state->a);
    state->flags.cy = result > UINT8_MAX ? 1 : 0;
    //TODO: Add auxiliary carry bit
}

void perform_adc(state_8080_t *state, const uint8_t operand) {
    uint16_t result = (uint16_t) state->a + (uint16_t) operand + (uint16_t) state->flags.cy;
    state->a = (uint8_t) (result & 0xff);

    state->flags.z = state->a == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(state->a);
    state->flags.p = get_parity_bit(state->a);
    state->flags.cy = result > UINT8_MAX ? 1 : 0;
    //TODO: Add auxiliary carry bit
}

void perform_sub(state_8080_t *state, const uint8_t operand) {
    uint16_t result = (uint16_t) state->a - (uint16_t) operand;
    state->a = (uint8_t) (result & 0xff);

    state->flags.z = state->a == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(state->a);
    state->flags.p = get_parity_bit(state->a);
    state->flags.cy = result > UINT8_MAX ? 0 : 1; //Borrow In is complement of Carry Out //TODO: Verify correctness of this statement
    //TODO: Add auxiliary carry bit
}

void perform_sbb(state_8080_t *state, const uint8_t operand) {
    uint16_t result = (uint16_t) state->a - (uint16_t) operand - (uint16_t) state->flags.cy;
    state->a = (uint8_t) (result & 0xff);

    state->flags.z = state->a == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(state->a);
    state->flags.p = get_parity_bit(state->a);
    state->flags.cy = result > UINT8_MAX ? 0 : 1; //Borrow In is complement of Carry Out //TODO: Verify correctness of this statement
    //TODO: Add auxiliary carry bit
}

void perform_ana(state_8080_t *state, const uint8_t operand) {
    state->a = state->a & operand;
    state->flags.z = state->a == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(state->a);
    state->flags.p = get_parity_bit(state->a);
    state->flags.cy = 0;
    //TODO: Add auxiliary carry bit
}

void perform_xra(state_8080_t *state, const uint8_t operand) {
    state->a = state->a ^ operand;
    state->flags.z = state->a == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(state->a);
    state->flags.p = get_parity_bit(state->a);
    state->flags.cy = 0;
    //TODO: Add auxiliary carry bit
}

void perform_ora(state_8080_t *state, const uint8_t operand) {
    state->a = state->a | operand;
    state->flags.z = state->a == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(state->a);
    state->flags.p = get_parity_bit(state->a);
    state->flags.cy = 0;
    //TODO: Add auxiliary carry bit
}

void perform_cmp(state_8080_t *state, const uint8_t operand) {
    uint16_t result = (uint16_t) state->a - (uint16_t) operand;

    state->flags.z = (result & 0xff) == 0 ? 1 : 0;
    state->flags.s = get_sign_bit((result & 0xff));
    state->flags.p = get_parity_bit((result & 0xff));
    state->flags.cy = result > UINT8_MAX ? 0 : 1; //Borrow In is complement of Carry Out //TODO: Verify correctness of this statement
    //TODO: Add auxiliary carry bit
}