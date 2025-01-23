#ifndef DRA_MIPS_C_INSTRUCTION_H
#define DRA_MIPS_C_INSTRUCTION_H

typedef enum {
	NOP = 0,
	ADD, ADDU, ADDI, ADDIU,
	SUB, SUBU, SUBI, SUBIU,
	MOVE, MOVS, MOVD,
	MFHI, MFLO,
	MULT, MUL,
	SYSCALL,
	NUM_OPCODES
} opcode_t;

// TODO : Maybe reimplement this with proper bitpacking? Altough the bitshifting for real architecture accurate instructions would make the software simulation slower... but each instruction would take up less space in memory, and that could potentially be more cache friendly, so who knows...
// In any case, packing with unions and bitfields, which would make our lives much easier, is not even guaranteed to work 100% as expected and give us full packing, so we'd have to do manual bit shifting.
typedef struct {
	int opcode;
	int data[3];
} instruction_t;

// These macros look dumb af right now, but they will make sense once we rework the internal representation of each instruction_t so that they take up less space. That way I won't have to go through rewriting all of the VM's code again...
#define instruction_opcode(instruction) (instruction.opcode)
#define instruction_r0(instruction) (instruction.r[0])
#define instruction_r1(instruction) (instruction.r[1])
#define instruction_r2(instruction) (instruction.r[2])

#endif
