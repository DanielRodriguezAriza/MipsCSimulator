#ifndef DRA_MIPS_C_INSTRUCTION_H
#define DRA_MIPS_C_INSTRUCTION_H

typedef enum {
	NOP = 0,
	ADD, ADDU, ADDI, ADDIU,
	SUB, SUBU, SUBI, SUBIU,
	MOVE, MOVS, MOVD,
	MFHI, MFLO,
	SYSCALL,
	NUM_OPCODES
} opcode_t;

typedef struct {
	unsigned char buffer[4]; // opcode, rdest, rorg1, rorg2
} instruction_t;

#endif
