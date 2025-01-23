#ifndef DRA_MIPS_C_INSTRUCTION_H
#define DRA_MIPS_C_INSTRUCTION_H

/*

	List of instructions:
	
	// Generic
	nop
	
	// Integer
	add $r0, $r1, $r2 // signed integer sum with overflow. $r0 = $r1 + $r2
	addu $r0, $r1, $r2 // unsigned integer sum with overflow. $ro = $r1 + $r2
	
	addi $r0, $r1, imm // signed integer immediate sum with overflow. $r0 = $r1 + imm
	addiu $r0, $r1, imm // unsigned integer immediate sum with overflow. $r0 = $r1 + imm
	
	sub $r0, $r1, $r2 // signed integer subtraction with overflow. $r0 = $r1 - $r2
	subu $r0, $r1, $r2 // unsigned integer subtraction with overflow. $r0 = $r1 - $r2
	
	subi $r0, $r1, imm // signed integer immediate subtraction with overflow. $r0 = $r1 - imm
	subiu $r0, $r1, imm // unsigned integer immediate subtraction with overflow. $r0 = $r1 - imm
	
	// Float
	

*/

// NOTE : muli and muliu don't really exist as instructions, but MARS offers a non standard mul overload that works with immediates, so I do the same but with new extended instructions for clearness. Also note that certain instructions like the standard mul are already pseudo instructions, but we implement them all through a real instruction of their own as of now because it is easier to implement and deal with...

typedef enum {
	NOP = 0,
	ADD, ADDU, ADDI, ADDIU,
	SUB, SUBU, SUBI, SUBIU,
	ADDS, ADDD,
	SUBS, SUBD,
	MOVE, MOVS, MOVD,
	MFHI, MFLO,
	MULT, MULTU, MUL, MULU, MULI, MULIU,
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
