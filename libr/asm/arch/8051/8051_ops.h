/* radare - LGPL - Copyright 2015-2016 - pancake, condret, riq, qnix */

#ifndef _8051_OPS_H
#define _8051_OPS_H

#include <r_types.h>

static inline ut16 arg_offset (ut16 pc, ut8 offset) {
	if (offset < 0x80) {
		return pc + offset;
	}
	offset = 0 - offset;
	return pc - offset;
}

static inline ut16 arg_addr11 (ut16 pc, const ut8 *buf) {
	// ADDR11 is replacing lower 11 bits of (pre-incremented) PC
	return (pc & 0xf800) + ((buf[0] & 0xe0) << 3) + buf[1];
}

static inline ut8 arg_bit (ut8 bit_addr) {
	if (bit_addr < 0x80) {
		// bit addresses 0x00-0x7f are mapped to bytes at 0x20-0x2f
		return (bit_addr >> 3) + 0x20;
	}
	// bit addresses 0x80-0xff are mapped to bytes at 0x80, 0x88, 0x98, ...
	return bit_addr & 0xf8;
}

typedef enum {
	OP_INVALID = 0,
	OP_ADD,
	OP_ADDC,
	OP_ANL,
	OP_CALL,
	OP_CJNE,
	OP_CLR,
	OP_CPL,
	OP_DA,
	OP_DEC,
	OP_DIV,
	OP_DJNZ,
	OP_INC,
	OP_JB,
	OP_JBC,
	OP_JC,
	OP_JMP,
	OP_JNB,
	OP_JNC,
	OP_JNZ,
	OP_JZ,
	OP_MOV,
	OP_MUL,
	OP_NOP,
	OP_ORL,
	OP_POP,
	OP_PUSH,
	OP_RET,
	OP_RL,
	OP_RLC,
	OP_RR,
	OP_RRC,
	OP_SETB,
	OP_SUBB,
	OP_SWAP,
	OP_XCH,
	OP_XRL
} op8051;

typedef enum {
	A_NONE = 0,
	A_RI,		// @Ri
	A_RN,		// Rn
	A_ADDR11,
	A_ADDR16,
	A_DIRECT,
	A_BIT,
	A_IMMEDIATE,
	A_IMM16,
	A_OFFSET
} argtype8051;

typedef enum {
	M_NONE = 0,
	M_RI = 0x01,
	M_RN = 0x07,
	M_ADDR11 = 0xe0
} argmask8051;

typedef struct {
	ut8 op;
	op8051 instr;	// instruction
	_RAnalOpType type;
	char* string;	// disassembly output
	size_t len;
	argmask8051 mask;	// bits masked to determine opcode
	argtype8051 arg1;
	argtype8051 arg2;
	argtype8051 arg3;
} _8051_op_t;

static _8051_op_t _8051_ops[] = {
#	define T(op) R_ANAL_OP_TYPE_ ## op
	{0x00, OP_NOP, T (NOP), "nop", 1, M_NONE, 0, 0, 0},
	{0x01, OP_JMP, T (JMP), "ajmp 0x%04x", 2, M_ADDR11, A_ADDR11, 0, 0},
	{0x02, OP_JMP, T (JMP), "ljmp 0x%04x", 3, M_NONE, A_ADDR16, 0, 0},
	{0x03, OP_RR, T (ROR), "rr a", 1, M_NONE, 0, 0, 0},
	{0x04, OP_INC, T (ADD), "inc a", 1, M_NONE, 0, 0, 0},
	{0x05, OP_INC, T (ADD), "inc 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x06, OP_INC, T (ADD), "inc @r%d", 1, M_RI, A_RI, 0, 0},
	{0x08, OP_INC, T (ADD), "inc r%d", 1, M_RN, A_RN, 0, 0},
	{0x10, OP_JBC, T (CJMP), "jbc 0x%02x.%d, 0x%04x", 3, M_NONE, A_BIT, A_OFFSET, 0},
	{0x11, OP_CALL, T (CALL), "acall 0x%04x", 2, M_ADDR11, A_ADDR11, 0, 0},
	{0x12, OP_CALL, T (CALL), "lcall 0x%04x", 3, M_NONE, A_ADDR16, 0, 0},
	{0x13, OP_RRC, T (ROR), "rrc a", 1, M_NONE, 0, 0, 0},
	{0x14, OP_DEC, T (SUB), "dec a", 1, M_NONE, 0, 0, 0},
	{0x15, OP_DEC, T (SUB), "dec 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x16, OP_DEC, T (SUB), "dec @r%d", 1, M_RI, A_RI, 0, 0, },
	{0x18, OP_DEC, T (SUB), "dec r%d", 1, M_RN, A_RN, 0, 0},
	{0x20, OP_JB, T (CJMP), "jb 0x%02x.%d, 0x%04x", 3, M_NONE, A_BIT, A_OFFSET, 0},
	{0x22, OP_RET, T (RET), "ret", 1, M_NONE, A_NONE, 0, 0},
	{0x23, OP_RL, T (ROL), "rl a", 1, M_NONE, A_NONE, 0, 0},
	{0x24, OP_ADD, T (ADD), "add a, #0x%02x", 2, M_NONE, A_IMMEDIATE, 0, 0},
	{0x25, OP_ADD, T (ADD), "add a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x26, OP_ADD, T (ADD), "add a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0x28, OP_ADD, T (ADD), "add a, r%d", 1, M_RN, A_RN, 0, 0},
	{0x30, OP_JNB, T (CJMP), "jnb 0x%02x.%d, 0x%04x", 3, M_NONE, A_BIT, A_OFFSET, 0},
	{0x32, OP_RET, T (RET), "reti", 1, M_NONE, 0, 0, 0},
	{0x33, OP_RLC, T (ROR), "rlc a", 1, M_NONE, 0, 0, 0},
	{0x34, OP_ADDC, T (ADD), "addc a, #0x%02x", 2, M_NONE, A_IMMEDIATE, 0, 0},
	{0x35, OP_ADDC, T (ADD), "addc a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x36, OP_ADDC, T (ADD), "addc a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0x38, OP_ADDC, T (ADD), "addc a, r%d", 1, M_RN, A_RN, 0, 0},
	{0x40, OP_JC, T (CJMP), "jc 0x%04x", 2, M_NONE, A_OFFSET, 0, 0},
	{0x42, OP_ORL, T (OR), "orl 0x%02x, a", 2, M_NONE, A_DIRECT, 0, 0},
	{0x43, OP_ORL, T (OR), "orl 0x%02x, #0x%02x", 3, M_NONE, A_DIRECT, A_IMMEDIATE, 0},
	{0x44, OP_ORL, T (OR), "orl a, #0x%02x", 2, M_NONE, A_IMMEDIATE, 0, 0},
	{0x45, OP_ORL, T (OR), "orl a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x46, OP_ORL, T (OR), "orl a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0x48, OP_ORL, T (OR), "orl a, r%d", 1, M_RN, A_RN, 0, 0},
	{0x50, OP_JNC, T (CJMP), "jnc 0x%04x", 2, M_NONE, A_OFFSET, 0, 0},
	{0x52, OP_ANL, T (AND), "anl 0x%02x, a", 2, M_NONE, A_DIRECT, 0, 0},
	{0x53, OP_ANL, T (AND), "anl 0x%02x, #0x%02x", 3, M_NONE, A_DIRECT, A_IMMEDIATE, 0},
	{0x54, OP_ANL, T (AND), "anl a, #0x%02x", 2, M_NONE, A_IMMEDIATE, 0, 0},
	{0x55, OP_ANL, T (AND), "anl a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x56, OP_ANL, T (AND), "anl a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0x58, OP_ANL, T (AND), "anl a, r%d", 1, M_RN, A_RN, 0, 0},
	{0x60, OP_JZ, T (CJMP), "jz 0x%04x", 2, M_NONE, A_OFFSET, 0, 0},
	{0x62, OP_XRL, T (XOR), "xrl 0x%02x, a", 2, M_NONE, A_DIRECT, 0, 0},
	{0x63, OP_XRL, T (XOR), "xrl 0x%02x, #0x%02x", 3, M_NONE, A_DIRECT, A_IMMEDIATE, 0},
	{0x64, OP_XRL, T (XOR), "xrl a, #0x%02x", 2, M_NONE, A_IMMEDIATE, 0, 0},
	{0x65, OP_XRL, T (XOR), "xrl a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x66, OP_XRL, T (XOR), "xrl a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0x68, OP_XRL, T (XOR), "xrl a, r%d", 1, M_RN, A_RN, 0, 0},
	{0x70, OP_JNZ, T (CJMP), "jnz 0x%04x", 2, M_NONE, A_OFFSET, 0, 0},
	{0x72, OP_ORL, T (OR), "orl c, 0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0x73, OP_JMP, T (UJMP), "jmp @a+dptr", 1, M_NONE, 0, 0, 0},
	{0x74, OP_MOV, T (MOV), "mov a, #0x%02x", 2, M_NONE, A_IMMEDIATE, 0, 0},
	{0x75, OP_MOV, T (MOV), "mov 0x%02x, #0x%02x", 3, M_NONE, A_DIRECT, A_IMMEDIATE, 0},
	{0x76, OP_MOV, T (MOV), "mov @r%d, #0x%02x", 2, M_RI, A_RI, A_IMMEDIATE, 0},
	{0x78, OP_MOV, T (MOV), "mov r%d, #0x%02x", 2, M_RN, A_RN, A_IMMEDIATE, 0},
	{0x80, OP_JMP, T (JMP), "sjmp 0x%04x", 2, M_NONE, A_OFFSET, 0, 0},
	{0x82, OP_ANL, T (AND), "anl c, 0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0x83, OP_MOV, T (MOV), "movc a, @a+pc", 1, M_NONE, 0, 0, 0},
	{0x84, OP_DIV, T (DIV), "div ab", 1, M_NONE, 0, 0, 0},
	{0x85, OP_MOV, T (MOV), "mov 0x%02x, 0x%02x", 3, M_NONE, A_DIRECT, A_DIRECT, 0},
	{0x86, OP_MOV, T (MOV), "mov 0x%02x, @r%d", 2, M_RI, A_DIRECT, A_RI, 0},
	{0x88, OP_MOV, T (MOV), "mov 0x%02x, r%d", 2, M_RN, A_DIRECT, A_RN, 0},
	{0x90, OP_MOV, T (MOV), "mov dptr, #0x%04x", 3, M_NONE, A_IMM16, 0, 0},
	{0x92, OP_MOV, T (MOV), "mov 0x%02x.%d, c", 2, M_NONE, A_BIT, 0, 0},
	{0x93, OP_MOV, T (MOV), "movc a, @a+dptr", 1, M_NONE, 0, 0, 0},
	{0x94, OP_SUBB, T (SUB), "subb a, #0x%02x", 2, M_NONE, A_IMMEDIATE, 0, 0},
	{0x95, OP_SUBB, T (SUB), "subb a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0x96, OP_SUBB, T (SUB), "subb a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0x98, OP_SUBB, T (SUB), "subb a, r%d", 1, M_RN, A_RN, 0, 0},
	{0xa0, OP_ORL, T (OR), "orl c, /0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0xa2, OP_MOV, T (MOV), "mov c, 0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0xa3, OP_INC, T (ADD), "inc dptr", 1, M_NONE, 0, 0, 0},
	{0xa4, OP_MUL, T (MUL), "mul ab", 1, M_NONE, 0, 0, 0},
	{0xa6, OP_MOV, T (MOV), "mov @r%d, 0x%02x", 2, M_RI, A_RI, A_DIRECT, 0},
	{0xa8, OP_MOV, T (MOV), "mov r%d, 0x%02x", 2, M_RN, A_RN, A_DIRECT, 0},
	{0xb0, OP_ANL, T (AND), "anl c, /0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0xb2, OP_CPL, T (CPL), "cpl 0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0xb3, OP_CPL, T (CPL), "cpl c", 1, M_NONE, 0, 0},
	{0xb4, OP_CJNE, T (CJMP), "cjne a, #0x%02x, 0x%04x", 3, M_NONE, A_IMMEDIATE, A_OFFSET, 0},
	{0xb5, OP_CJNE, T (CJMP), "cjne a, 0x%02x, 0x%04x", 3, M_NONE, A_DIRECT, A_OFFSET, 0},
	{0xb6, OP_CJNE, T (CJMP) | T (IND), "cjne @r%d, #0x%02x, 0x%04x", 3, M_RI, A_RI, A_IMMEDIATE, A_OFFSET},
	{0xb8, OP_CJNE, T (CJMP), "cjne r%d, #0x%02x, 0x%04x", 3, M_RN, A_RN, A_IMMEDIATE, A_OFFSET},
	{0xc0, OP_PUSH, T (PUSH) | T (MEM), "push 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0xc2, OP_CLR, T (IO), "clr 0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0xc3, OP_CLR, T (IO), "clr c", 1, M_NONE, 0, 0, 0},
	{0xc4, OP_SWAP, T (XCHG), "swap a", 1, M_NONE, 0, 0, 0},
	{0xc5, OP_XCH, T (XCHG), "xch a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0xc6, OP_XCH, T (XCHG), "xch a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0xc8, OP_XCH, T (XCHG), "xch a, r%d", 1, M_RN, A_RN, 0, 0},
	{0xd0, OP_POP, T (POP) | T (MEM), "pop 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0xd2, OP_SETB, T (IO), "setb 0x%02x.%d", 2, M_NONE, A_BIT, 0, 0},
	{0xd3, OP_SETB, T (IO), "setb c", 1, M_NONE, 0, 0, 0},
	{0xd4, OP_DA, T (CAST), "da a", 1, M_NONE, 0, 0, 0},
	{0xd5, OP_DJNZ, T (CJMP), "djnz 0x%02x, 0x%04x", 3, M_NONE, A_DIRECT, A_OFFSET, 0},
	{0xd6, OP_XCH, T (XCHG), "xchd a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0xd8, OP_DJNZ, T (CJMP), "djnz r%d, 0x%04x", 2, M_RN, A_RN, A_OFFSET, 0},
	{0xe0, OP_MOV, T (MOV), "movx a, @dptr", 1, M_NONE, 0, 0, 0},
	{0xe2, OP_MOV, T (MOV), "movx a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0xe4, OP_CLR, T (IO), "clr a", 1, M_NONE, 0, 0, 0},
	{0xe5, OP_MOV, T (MOV), "mov a, 0x%02x", 2, M_NONE, A_DIRECT, 0, 0},
	{0xe6, OP_MOV, T (MOV), "mov a, @r%d", 1, M_RI, A_RI, 0, 0},
	{0xe8, OP_MOV, T (MOV), "mov a, r%d", 1, M_RN, A_RN, 0, 0},
	{0xf0, OP_MOV, T (MOV), "movx @dptr, a", 1, M_NONE, 0, 0, 0},
	{0xf2, OP_MOV, T (MOV), "movx @r%d, a", 1, M_RI, A_RI, 0, 0},
	{0xf4, OP_CPL, T (CPL), "cpl a", 1, M_NONE, 0, 0, 0},
	{0xf5, OP_MOV, T (MOV), "mov 0x%02x, a", 2, M_NONE, A_DIRECT, 0, 0},
	{0xf6, OP_MOV, T (MOV), "mov @r%d, a", 1, M_RI, A_RI, 0, 0},
	{0xf8, OP_MOV, T (MOV), "mov r%d, a", 1, M_RN, A_RN, 0, 0},
	{0xff, OP_INVALID, T (ILL), NULL, 0, 0, 0}
#	undef T
};

#endif

