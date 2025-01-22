#include <stdio.h>

typedef enum {
	NOP = 0,
	LI,
	ADD, SUB, MUL, DIV, MOD,
	J, JEQ, JNE, JLT, JGT, JLE, JGE,
	JR, JREQ, JRNE, JRLT, JRGT, JRLE, JRGE,
	MOV,
	SYS,
	NUM_INSTRUCTIONS,
} instruction_code_t; // opcodes

typedef struct {
	unsigned char buffer[4];
} instruction_t;

static int registers[32] = {0};

void execute_syscall(instruction_t instruction)
{
	// TODO : Obviously rework, syscalls should not really be instructions like the others or even take fucking arguments as params... or maybe it's ok in this custom made up VM architecture, who knows!
	switch(instruction.buffer[1])
	{
		case 0: // code for print_int syscall
			printf("r%d = %d\n", instruction.buffer[2], registers[instruction.buffer[2]]);
			break;
	}
}

void execute_instructions(instruction_t *instructions, size_t count)
{
	size_t program_counter = 0;
	while(program_counter < count)
	{
		instruction_t instruction = instructions[program_counter];
		switch(instruction.buffer[0])
		{
			case NOP:
				break;
			case LI:
				registers[instruction.buffer[1]] = *(short*)(&(instruction.buffer[2]));
				break;
			case MOV:
				registers[instruction.buffer[1]] = registers[instruction.buffer[2]];
				break;
			case ADD:
				registers[instruction.buffer[1]] = registers[instruction.buffer[2]] + registers[instruction.buffer[3]];
				break;
			case SUB:
				registers[instruction.buffer[1]] = registers[instruction.buffer[2]] - registers[instruction.buffer[3]];
				break;
			case MUL:
				registers[instruction.buffer[1]] = registers[instruction.buffer[2]] * registers[instruction.buffer[3]];
				break;
			case DIV:
				registers[instruction.buffer[1]] = registers[instruction.buffer[2]] / registers[instruction.buffer[3]];
				break;
			case MOD:
				registers[instruction.buffer[1]] = registers[instruction.buffer[2]] % registers[instruction.buffer[3]];
				break;
			case J:
				program_counter = instruction.buffer[1] - 1;
				break;
			case JEQ:
				if(registers[instruction.buffer[1]] == registers[instruction.buffer[2]])
					program_counter = instruction.buffer[3] - 1;
				break;
			case JNE:
				if(registers[instruction.buffer[1]] != registers[instruction.buffer[2]])
					program_counter = instruction.buffer[3] - 1;
				break;
			case JLT:
				if(registers[instruction.buffer[1]] < registers[instruction.buffer[2]])
					program_counter = instruction.buffer[3] - 1;
				break;
			case JGT:
				if(registers[instruction.buffer[1]] > registers[instruction.buffer[2]])
					program_counter = instruction.buffer[3] - 1;
				break;
			case JLE:
				if(registers[instruction.buffer[1]] <= registers[instruction.buffer[2]])
					program_counter = instruction.buffer[3] - 1;
				break;
			case JGE:
				if(registers[instruction.buffer[1]] >= registers[instruction.buffer[2]])
					program_counter = instruction.buffer[3] - 1;
				break;
			case SYS:
				execute_syscall(instruction);
				break;
			case JR:
				program_counter = registers[instruction.buffer[1]] - 1;
				break;
			case JREQ:
				if(registers[instruction.buffer[1]] == registers[instruction.buffer[2]])
					program_counter = registers[instruction.buffer[3]] - 1;
				break;
			case JRNE:
				if(registers[instruction.buffer[1]] != registers[instruction.buffer[2]])
					program_counter = registers[instruction.buffer[3]] - 1;
				break;
			case JRLT:
				if(registers[instruction.buffer[1]] < registers[instruction.buffer[2]])
					program_counter = registers[instruction.buffer[3]] - 1;
				break;
			case JRGT:
				if(registers[instruction.buffer[1]] > registers[instruction.buffer[2]])
					program_counter = registers[instruction.buffer[3]] - 1;
				break;
			case JRLE:
				if(registers[instruction.buffer[1]] <= registers[instruction.buffer[2]])
					program_counter = registers[instruction.buffer[3]] - 1;
				break;
			case JRGE:
				if(registers[instruction.buffer[1]] >= registers[instruction.buffer[2]])
					program_counter = registers[instruction.buffer[3]] - 1;
				break;
			default:
				printf("Unknown instruction found!\n");
				break;
		}
		program_counter++;
	}
}

int main()
{
	instruction_t instructions[] = {
		{LI, 0, 0},
		{LI, 1, 1},
		{LI, 2, 10},
		{SUB, 2, 2, 1},
		{SYS, 0, 2},
		{JGT, 2, 0, 3},
	};
	size_t count = sizeof(instructions) / sizeof(instructions[0]);
	execute_instructions(instructions, count);
}
