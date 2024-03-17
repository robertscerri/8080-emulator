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
void perform_inr(state_8080_t *state, uint8_t *reg);
void perform_dcr(state_8080_t *state, uint8_t *reg);
void perform_inx(state_8080_t *state, uint8_t *msb, uint8_t *lsb);
void perform_dcx(state_8080_t *state, uint8_t *msb, uint8_t *lsb);
void perform_add(state_8080_t *state, const uint8_t operand);
void perform_adc(state_8080_t *state, const uint8_t operand);
void perform_sub(state_8080_t *state, const uint8_t operand);
void perform_sbb(state_8080_t *state, const uint8_t operand);
void perform_dad(state_8080_t *state, const uint16_t operand);

//Logical Group
void perform_ana(state_8080_t *state, const uint8_t operand);
void perform_xra(state_8080_t *state, const uint8_t operand);
void perform_ora(state_8080_t *state, const uint8_t operand);
void perform_cmp(state_8080_t *state, const uint8_t operand);

void perform_ret(state_8080_t *state);
void perform_call(state_8080_t *state, uint16_t addr);

void unimplemented_instruction(state_8080_t *state, unsigned char opcode) {
    printf("ERROR: Unimplemented instruction (0x%02x)\n", opcode);
    exit(1);
}

int Disassemble8080Op(unsigned char *codebuffer, int pc)
{
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf("%04x ", pc);
    switch (*code)
    {
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI    B,#$%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x02: printf("STAX   B"); break;
        case 0x03: printf("INX    B"); break;
        case 0x04: printf("INR    B"); break;
        case 0x05: printf("DCR    B"); break;
        case 0x06: printf("MVI    B,#$%02x", code[1]); opbytes=2; break;
        case 0x07: printf("RLC"); break;
        case 0x08: printf("NOP"); break;
        case 0x09: printf("DAD    B"); break;
        case 0x0a: printf("LDAX   B"); break;
        case 0x0b: printf("DCX    B"); break;
        case 0x0c: printf("INR    C"); break;
        case 0x0d: printf("DCR    C"); break;
        case 0x0e: printf("MVI    C,#$%02x", code[1]); opbytes = 2;	break;
        case 0x0f: printf("RRC"); break;

        case 0x10: printf("NOP"); break;
        case 0x11: printf("LXI    D,#$%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x12: printf("STAX   D"); break;
        case 0x13: printf("INX    D"); break;
        case 0x14: printf("INR    D"); break;
        case 0x15: printf("DCR    D"); break;
        case 0x16: printf("MVI    D,#$%02x", code[1]); opbytes=2; break;
        case 0x17: printf("RAL"); break;
        case 0x18: printf("NOP"); break;
        case 0x19: printf("DAD    D"); break;
        case 0x1a: printf("LDAX   D"); break;
        case 0x1b: printf("DCX    D"); break;
        case 0x1c: printf("INR    E"); break;
        case 0x1d: printf("DCR    E"); break;
        case 0x1e: printf("MVI    E,#$%02x", code[1]); opbytes = 2; break;
        case 0x1f: printf("RAR"); break;

        case 0x20: printf("NOP"); break;
        case 0x21: printf("LXI    H,#$%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x22: printf("SHLD   $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x23: printf("INX    H"); break;
        case 0x24: printf("INR    H"); break;
        case 0x25: printf("DCR    H"); break;
        case 0x26: printf("MVI    H,#$%02x", code[1]); opbytes=2; break;
        case 0x27: printf("DAA"); break;
        case 0x28: printf("NOP"); break;
        case 0x29: printf("DAD    H"); break;
        case 0x2a: printf("LHLD   $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x2b: printf("DCX    H"); break;
        case 0x2c: printf("INR    L"); break;
        case 0x2d: printf("DCR    L"); break;
        case 0x2e: printf("MVI    L,#$%02x", code[1]); opbytes = 2; break;
        case 0x2f: printf("CMA"); break;

        case 0x30: printf("NOP"); break;
        case 0x31: printf("LXI    SP,#$%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x32: printf("STA    $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x33: printf("INX    SP"); break;
        case 0x34: printf("INR    M"); break;
        case 0x35: printf("DCR    M"); break;
        case 0x36: printf("MVI    M,#$%02x", code[1]); opbytes=2; break;
        case 0x37: printf("STC"); break;
        case 0x38: printf("NOP"); break;
        case 0x39: printf("DAD    SP"); break;
        case 0x3a: printf("LDA    $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x3b: printf("DCX    SP"); break;
        case 0x3c: printf("INR    A"); break;
        case 0x3d: printf("DCR    A"); break;
        case 0x3e: printf("MVI    A,#$%02x", code[1]); opbytes = 2; break;
        case 0x3f: printf("CMC"); break;

        case 0x40: printf("MOV    B,B"); break;
        case 0x41: printf("MOV    B,C"); break;
        case 0x42: printf("MOV    B,D"); break;
        case 0x43: printf("MOV    B,E"); break;
        case 0x44: printf("MOV    B,H"); break;
        case 0x45: printf("MOV    B,L"); break;
        case 0x46: printf("MOV    B,M"); break;
        case 0x47: printf("MOV    B,A"); break;
        case 0x48: printf("MOV    C,B"); break;
        case 0x49: printf("MOV    C,C"); break;
        case 0x4a: printf("MOV    C,D"); break;
        case 0x4b: printf("MOV    C,E"); break;
        case 0x4c: printf("MOV    C,H"); break;
        case 0x4d: printf("MOV    C,L"); break;
        case 0x4e: printf("MOV    C,M"); break;
        case 0x4f: printf("MOV    C,A"); break;

        case 0x50: printf("MOV    D,B"); break;
        case 0x51: printf("MOV    D,C"); break;
        case 0x52: printf("MOV    D,D"); break;
        case 0x53: printf("MOV    D.E"); break;
        case 0x54: printf("MOV    D,H"); break;
        case 0x55: printf("MOV    D,L"); break;
        case 0x56: printf("MOV    D,M"); break;
        case 0x57: printf("MOV    D,A"); break;
        case 0x58: printf("MOV    E,B"); break;
        case 0x59: printf("MOV    E,C"); break;
        case 0x5a: printf("MOV    E,D"); break;
        case 0x5b: printf("MOV    E,E"); break;
        case 0x5c: printf("MOV    E,H"); break;
        case 0x5d: printf("MOV    E,L"); break;
        case 0x5e: printf("MOV    E,M"); break;
        case 0x5f: printf("MOV    E,A"); break;

        case 0x60: printf("MOV    H,B"); break;
        case 0x61: printf("MOV    H,C"); break;
        case 0x62: printf("MOV    H,D"); break;
        case 0x63: printf("MOV    H.E"); break;
        case 0x64: printf("MOV    H,H"); break;
        case 0x65: printf("MOV    H,L"); break;
        case 0x66: printf("MOV    H,M"); break;
        case 0x67: printf("MOV    H,A"); break;
        case 0x68: printf("MOV    L,B"); break;
        case 0x69: printf("MOV    L,C"); break;
        case 0x6a: printf("MOV    L,D"); break;
        case 0x6b: printf("MOV    L,E"); break;
        case 0x6c: printf("MOV    L,H"); break;
        case 0x6d: printf("MOV    L,L"); break;
        case 0x6e: printf("MOV    L,M"); break;
        case 0x6f: printf("MOV    L,A"); break;

        case 0x70: printf("MOV    M,B"); break;
        case 0x71: printf("MOV    M,C"); break;
        case 0x72: printf("MOV    M,D"); break;
        case 0x73: printf("MOV    M.E"); break;
        case 0x74: printf("MOV    M,H"); break;
        case 0x75: printf("MOV    M,L"); break;
        case 0x76: printf("HLT");        break;
        case 0x77: printf("MOV    M,A"); break;
        case 0x78: printf("MOV    A,B"); break;
        case 0x79: printf("MOV    A,C"); break;
        case 0x7a: printf("MOV    A,D"); break;
        case 0x7b: printf("MOV    A,E"); break;
        case 0x7c: printf("MOV    A,H"); break;
        case 0x7d: printf("MOV    A,L"); break;
        case 0x7e: printf("MOV    A,M"); break;
        case 0x7f: printf("MOV    A,A"); break;

        case 0x80: printf("ADD    B"); break;
        case 0x81: printf("ADD    C"); break;
        case 0x82: printf("ADD    D"); break;
        case 0x83: printf("ADD    E"); break;
        case 0x84: printf("ADD    H"); break;
        case 0x85: printf("ADD    L"); break;
        case 0x86: printf("ADD    M"); break;
        case 0x87: printf("ADD    A"); break;
        case 0x88: printf("ADC    B"); break;
        case 0x89: printf("ADC    C"); break;
        case 0x8a: printf("ADC    D"); break;
        case 0x8b: printf("ADC    E"); break;
        case 0x8c: printf("ADC    H"); break;
        case 0x8d: printf("ADC    L"); break;
        case 0x8e: printf("ADC    M"); break;
        case 0x8f: printf("ADC    A"); break;

        case 0x90: printf("SUB    B"); break;
        case 0x91: printf("SUB    C"); break;
        case 0x92: printf("SUB    D"); break;
        case 0x93: printf("SUB    E"); break;
        case 0x94: printf("SUB    H"); break;
        case 0x95: printf("SUB    L"); break;
        case 0x96: printf("SUB    M"); break;
        case 0x97: printf("SUB    A"); break;
        case 0x98: printf("SBB    B"); break;
        case 0x99: printf("SBB    C"); break;
        case 0x9a: printf("SBB    D"); break;
        case 0x9b: printf("SBB    E"); break;
        case 0x9c: printf("SBB    H"); break;
        case 0x9d: printf("SBB    L"); break;
        case 0x9e: printf("SBB    M"); break;
        case 0x9f: printf("SBB    A"); break;

        case 0xa0: printf("ANA    B"); break;
        case 0xa1: printf("ANA    C"); break;
        case 0xa2: printf("ANA    D"); break;
        case 0xa3: printf("ANA    E"); break;
        case 0xa4: printf("ANA    H"); break;
        case 0xa5: printf("ANA    L"); break;
        case 0xa6: printf("ANA    M"); break;
        case 0xa7: printf("ANA    A"); break;
        case 0xa8: printf("XRA    B"); break;
        case 0xa9: printf("XRA    C"); break;
        case 0xaa: printf("XRA    D"); break;
        case 0xab: printf("XRA    E"); break;
        case 0xac: printf("XRA    H"); break;
        case 0xad: printf("XRA    L"); break;
        case 0xae: printf("XRA    M"); break;
        case 0xaf: printf("XRA    A"); break;

        case 0xb0: printf("ORA    B"); break;
        case 0xb1: printf("ORA    C"); break;
        case 0xb2: printf("ORA    D"); break;
        case 0xb3: printf("ORA    E"); break;
        case 0xb4: printf("ORA    H"); break;
        case 0xb5: printf("ORA    L"); break;
        case 0xb6: printf("ORA    M"); break;
        case 0xb7: printf("ORA    A"); break;
        case 0xb8: printf("CMP    B"); break;
        case 0xb9: printf("CMP    C"); break;
        case 0xba: printf("CMP    D"); break;
        case 0xbb: printf("CMP    E"); break;
        case 0xbc: printf("CMP    H"); break;
        case 0xbd: printf("CMP    L"); break;
        case 0xbe: printf("CMP    M"); break;
        case 0xbf: printf("CMP    A"); break;

        case 0xc0: printf("RNZ"); break;
        case 0xc1: printf("POP    B"); break;
        case 0xc2: printf("JNZ    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xc3: printf("JMP    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xc4: printf("CNZ    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xc5: printf("PUSH   B"); break;
        case 0xc6: printf("ADI    #$%02x",code[1]); opbytes = 2; break;
        case 0xc7: printf("RST    0"); break;
        case 0xc8: printf("RZ"); break;
        case 0xc9: printf("RET"); break;
        case 0xca: printf("JZ     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xcb: printf("JMP    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xcc: printf("CZ     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xcd: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xce: printf("ACI    #$%02x",code[1]); opbytes = 2; break;
        case 0xcf: printf("RST    1"); break;

        case 0xd0: printf("RNC"); break;
        case 0xd1: printf("POP    D"); break;
        case 0xd2: printf("JNC    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xd3: printf("OUT    #$%02x",code[1]); opbytes = 2; break;
        case 0xd4: printf("CNC    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xd5: printf("PUSH   D"); break;
        case 0xd6: printf("SUI    #$%02x",code[1]); opbytes = 2; break;
        case 0xd7: printf("RST    2"); break;
        case 0xd8: printf("RC");  break;
        case 0xd9: printf("RET"); break;
        case 0xda: printf("JC     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xdb: printf("IN     #$%02x",code[1]); opbytes = 2; break;
        case 0xdc: printf("CC     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xdd: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xde: printf("SBI    #$%02x",code[1]); opbytes = 2; break;
        case 0xdf: printf("RST    3"); break;

        case 0xe0: printf("RPO"); break;
        case 0xe1: printf("POP    H"); break;
        case 0xe2: printf("JPO    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xe3: printf("XTHL");break;
        case 0xe4: printf("CPO    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xe5: printf("PUSH   H"); break;
        case 0xe6: printf("ANI    #$%02x",code[1]); opbytes = 2; break;
        case 0xe7: printf("RST    4"); break;
        case 0xe8: printf("RPE"); break;
        case 0xe9: printf("PCHL");break;
        case 0xea: printf("JPE    $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xeb: printf("XCHG"); break;
        case 0xec: printf("CPE     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xed: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xee: printf("XRI    #$%02x",code[1]); opbytes = 2; break;
        case 0xef: printf("RST    5"); break;

        case 0xf0: printf("RP");  break;
        case 0xf1: printf("POP    PSW"); break;
        case 0xf2: printf("JP     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xf3: printf("DI");  break;
        case 0xf4: printf("CP     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xf5: printf("PUSH   PSW"); break;
        case 0xf6: printf("ORI    #$%02x",code[1]); opbytes = 2; break;
        case 0xf7: printf("RST    6"); break;
        case 0xf8: printf("RM");  break;
        case 0xf9: printf("SPHL");break;
        case 0xfa: printf("JM     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xfb: printf("EI");  break;
        case 0xfc: printf("CM     $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xfd: printf("CALL   $%02x%02x",code[2],code[1]); opbytes = 3; break;
        case 0xfe: printf("CPI    #$%02x",code[1]); opbytes = 2; break;
        case 0xff: printf("RST    7"); break;
    }

    return opbytes;
}

int emulate_8080(state_8080_t *state) {
    unsigned char *opcode = &state->memory[state->pc];

    Disassemble8080Op(state->memory, state->pc);

    state->pc++;

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
        case 0x09:
            perform_dad(state, get_2byte_word(state->b, state->c));
            break;
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
        case 0x19:
            perform_dad(state, get_2byte_word(state->d, state->e));
            break;
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
        case 0x1f:
            {
                uint8_t prevBit0 = state->a & 0x01;
                uint8_t prevBit7 = state->a & 0x80;
                state->a = state->a >> 1;
                state->a = state->a | prevBit7;
                state->flags.cy = prevBit0;
                break;
            }
        //TODO: Implement opcode 0x20 (RIM)
        case 0x21:
            state->h = opcode[2];
            state->l = opcode[1];
            state->pc += 2;
            break;
        case 0x22:
            {
                uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                state->memory[addr] = state->l;
                state->memory[addr + 1] = state->h;
                state->pc += 2;
                break;
            }
        case 0x23:
            perform_inx(state, &state->h, &state->l);
            break;
        case 0x24:
            perform_inr(state, &state->h);
            break;
        case 0x25:
            perform_dcr(state, &state->h);
            break;
        case 0x26:
            state->h = opcode[1];
            state->pc += 1;
            break;
        //TODO: Implement opcode 0x27
        case 0x29:
            perform_dad(state, get_2byte_word(state->h, state->l));
            break;
        case 0x2a:
            {
                uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                state->l = state->memory[addr];
                state->h = state->memory[addr + 1];
                state->pc += 2;
                break;
            }
        case 0x2b:
            perform_dcx(state, &state->h, &state->l);
            break;
        case 0x2c:
            perform_inr(state, &state->l);
            break;
        case 0x2d:
            perform_dcr(state, &state->l);
            break;
        case 0x2e:
            state->l = opcode[1];
            state->pc += 1;
            break;
        case 0x2f:
            state->a = ~state->a;
            break;
        //TODO: Implement opcode 0x30 (SIM)
        case 0x31:
            state->sp = get_2byte_word(opcode[2], opcode[1]);
            state->pc += 2;
            break;
        case 0x32:
            {
                uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                state->memory[addr] = state->a;
                state->pc += 2;
                break;
            }
        case 0x33:
            state->sp++;
            break;
        case 0x34:
            state->memory[get_2byte_word(state->h, state->l)]++;
            break;
        case 0x35:
            state->memory[get_2byte_word(state->h, state->l)]--;
            break;
        case 0x36:
            state->memory[get_2byte_word(state->h, state->l)] = opcode[1];
            state->pc += 1;
            break;
        case 0x37:
            state->flags.cy = 1;
            break;
        case 0x39:
            perform_dad(state, state->sp);
            break;
        case 0x3a:
            {
                uint16_t addr = opcode[1] | (opcode[2] << 8);
                state->a = state->memory[addr];
                state->pc += 2;
                break;
            }
        case 0x3b:
            state->sp--;
            break;
        case 0x3c:
            perform_inr(state, &state->a);
            break;
        case 0x3d:
            perform_dcr(state, &state->a);
            break;
        case 0x3e:
            state->a = opcode[1];
            state->pc += 1;
            break;
        case 0x3f:
            state->flags.cy = ~state->flags.cy;
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
            return 1;
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
        case 0xc0:
            if (state->flags.z == 0) {
                perform_ret(state);
            }
            break;
        case 0xc1:
            state->c = state->memory[state->sp];
            state->b = state->memory[state->sp + 1];
            state->sp += 2;
            break;
        case 0xc2:
            {
                if (state->flags.z == 0) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        case 0xc3:
            {
                uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                state->pc = addr;
                break;
            }
        case 0xc4:
            if (state->flags.z == 0) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        case 0xc5:
            state->memory[state->sp - 2] = state->c;
            state->memory[state->sp - 1] = state->b;
            state->sp -= 2;
            break;
        case 0xc6:
            perform_add(state, opcode[1]);
            state->pc += 1;
            break;
        //TODO: Implement opcode 0xc7
        case 0xc8:
            if (state->flags.z == 1) {
                perform_ret(state);
            }
            break;
        case 0xc9:
            perform_ret(state);
            break;
        case 0xca:
            {
                if (state->flags.z == 1) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        //TODO: Implement opcode 0xcb
        case 0xcc:
            if (state->flags.z == 1) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        case 0xcd:
            perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            break;
        case 0xce:
            perform_adc(state, opcode[1]);
            state->pc += 1;
            break;
        //TODO: Implement opcode 0xcf
        case 0xd0:
            if (state->flags.cy == 0) {
                perform_ret(state);
            }
            break;
        case 0xd1:
            state->e = state->memory[state->sp];
            state->d = state->memory[state->sp + 1];
            state->sp += 2;
            break;
        case 0xd2:
            {
                if (state->flags.cy == 0) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        case 0xd3:
            //TODO: Add logic for OUT D8 (0xd3)
            state->pc += 1;
            break;
        case 0xd4:
            if (state->flags.cy == 0) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        case 0xd5:
            state->memory[state->sp - 2] = state->e;
            state->memory[state->sp - 1] = state->d;
            state->sp -= 2;
            break;
        //TODO: Implement opcodes 0xd6 -> 0xd7
        case 0xd8:
            if (state->flags.cy == 1) {
                perform_ret(state);
            }
            break;
        //TODO: Implement opcode 0xd9
        case 0xda:
            {
                if (state->flags.cy == 1) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        case 0xdb:
            //TODO: Add logic for IN D8 (0xd3)
            state->pc += 1;
            break;
        case 0xdc:
            if (state->flags.cy == 1) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        //TODO: Implement opcodes 0xdd -> 0xdf
        case 0xe0:
            if (state->flags.p == 0) {
                perform_ret(state);
            }
            break;
        case 0xe1:
            state->l = state->memory[state->sp];
            state->h = state->memory[state->sp + 1];
            state->sp += 2;
            break;
        case 0xe2:
            {
                if (state->flags.p == 0) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        case 0xe3:
            {
                int temp = state->memory[state->sp];
                state->memory[state->sp] = state->l;
                state->l = temp;

                temp = state->memory[state->sp + 1];
                state->memory[state->sp + 1] = state->h;
                state->h = temp;
                break;
            }
        case 0xe4:
            if (state->flags.p == 0) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        case 0xe5:
            state->memory[state->sp - 2] = state->l;
            state->memory[state->sp - 1] = state->h;
            state->sp -= 2;
            break;
        case 0xe6:
            perform_ana(state, opcode[1]);
            state->pc += 1;
            break;
        //TODO: Implement opcode 0xe7
        case 0xe8:
            if (state->flags.p == 1) {
                perform_ret(state);
            }
            break;
        //TODO: Implement opcode 0xe9
        case 0xea:
            {
                if (state->flags.p == 1) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        case 0xeb:
            {
                int temp = state->e;
                state->e = state->l;
                state->l = temp;

                temp = state->d;
                state->d = state->h;
                state->h = temp;
                break;
            }
        case 0xec:
            if (state->flags.p == 1) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        //TODO: Implement opcodes 0xed -> 0xef
        case 0xf0:
            if (state->flags.s == 0) {
                perform_ret(state);
            }
            break;
        case 0xf1:
            {
                uint8_t psw = state->memory[state->sp];
                state->flags.z = (0x01 == (psw & 0x01));
                state->flags.s = (0x02 == (psw & 0x02));
                state->flags.p = (0x04 == (psw & 0x04));
                state->flags.cy = (0x08 == (psw & 0x08));
                state->flags.ac = (0x10 == (psw & 0x10));
                state->a = state->memory[state->sp + 1];
                state->sp += 2;
                break;
            }
        case 0xf2:
            {
                if (state->flags.s == 0) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        case 0xf3:
            state->int_enable = 0;
            break;
        case 0xf4:
            if (state->flags.s == 0) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        case 0xf5:
            {
                state->memory[state->sp - 1] = state->a;
                uint8_t psw = (state->flags.z | state->flags.s << 1 | state->flags.p << 2 | state->flags.cy << 3 | state->flags.ac << 4);
                state->memory[state->sp - 2] = psw;
                state->sp -= 2;
                break;
            }
        //TODO: Implement opcodes 0xf6 -> 0xf7
        case 0xf8:
            if (state->flags.s == 1) {
                perform_ret(state);
            }
            break;
        case 0xf9:
            state->sp = get_2byte_word(state->h, state->l);
            break;
        case 0xfa:
            {
                if (state->flags.s == 1) {
                    uint16_t addr = get_2byte_word(opcode[2], opcode[1]);
                    state->pc = addr;
                } else {
                    state->pc += 2;
                }
                break;
            }
        case 0xfb:
            state->int_enable = 1;
            break;
        case 0xfc:
            if (state->flags.s == 1) {
                perform_call(state, get_2byte_word(opcode[2], opcode[1]));
            } else {
                state->pc += 2;
            }
            break;
        //TODO: Implement opcode 0xfd
        case 0xfe:
            perform_cmp(state, opcode[1]);
            state->pc += 1;
            break;
        //TODO: Implement opcode 0xff
        default:
            unimplemented_instruction(state, *opcode);
            break;
    }

    printf("\t");
    printf("%c", state->flags.z ? 'z' : '.');
    printf("%c", state->flags.s ? 's' : '.');
    printf("%c", state->flags.p ? 'p' : '.');
    printf("%c", state->flags.cy ? 'c' : '.');
    printf("%c  ", state->flags.ac ? 'a' : '.');
    printf("A $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", state->a, state->b, state->c,
           state->d, state->e, state->h, state->l, state->sp);

    return 0;
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

void perform_inr(state_8080_t *state, uint8_t *reg) {
    (*reg)++;

    state->flags.z = *reg == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(*reg);
    state->flags.p = get_parity_bit(*reg);
    //TODO: Add auxiliary carry bit
}

void perform_dcr(state_8080_t *state, uint8_t *reg) {
    (*reg)--;

    state->flags.z = *reg == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(*reg);
    state->flags.p = get_parity_bit(*reg);
    //TODO: Add auxiliary carry bit
}

void perform_inx(state_8080_t *state, uint8_t *msb, uint8_t *lsb) {
    uint16_t result = get_2byte_word(*msb, *lsb);
    result++;

    *msb = (result >> 8) & 0xff;
    *lsb = result & 0xff;
}

void perform_dcx(state_8080_t *state, uint8_t *msb, uint8_t *lsb) {
    uint16_t result = get_2byte_word(*msb, *lsb);
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

void perform_dad(state_8080_t *state, const uint16_t operand) {
    uint16_t contentsHL = get_2byte_word(state->h, state->l);
    uint32_t result = (uint32_t) contentsHL + (uint32_t) operand;

    state->h = (uint8_t) ((result & 0xff00) >> 8);
    state->l = (uint8_t) (result & 0xff);

    state->flags.z = result == 0 ? 1 : 0;
    state->flags.s = get_sign_bit(result);
    state->flags.p = get_parity_bit(result);
    state->flags.cy = result > UINT16_MAX ? 1 : 0;
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

void perform_ret(state_8080_t *state) {
    state->pc = get_2byte_word(state->memory[state->sp + 1], state->memory[state->sp]);
    state->sp += 2;
}

void perform_call(state_8080_t *state, uint16_t addr) {
    uint16_t ret_addr = state->pc + 2;
    state->memory[state->sp - 1] = (ret_addr & 0xff00) >> 8;
    state->memory[state->sp - 2] = (ret_addr & 0xff);
    state->sp -= 2;
    state->pc = addr;
}

void fread_to_mem(state_8080_t *state, char *filename, uint32_t offset) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("ERROR: Couldn't open %s\n", filename);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *buffer = &state->memory[offset];
    fread(buffer, fsize, 1, f);
    fclose(f);
}

state_8080_t *init8080(void) {
    state_8080_t *state = calloc(1, sizeof(state_8080_t));
    if (state == NULL) {
        printf("ERROR: Couldn't initialise state");
        exit(1);
    }

    state->memory = malloc(0x10000);
    if (state->memory == NULL) {
        printf("ERROR: Couldn't initialise memory");
        exit(1);
    }

    return state;
}

int main(void) {
    int done = 0;
    state_8080_t *state = init8080();

    fread_to_mem(state, "invaders.h", 0);
    fread_to_mem(state, "invaders.g", 0x800);
    fread_to_mem(state, "invaders.f", 0x1000);
    fread_to_mem(state, "invaders.e", 0x1800);

    while (done == 0) {
        done = emulate_8080(state);
    }

    free(state->memory);
    free(state);

    return 0;
}