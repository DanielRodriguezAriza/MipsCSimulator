#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SYSCALL_PRINT_INTEGER 1
#define SYSCALL_PRINT_FLOAT 2
#define SYSCALL_PRINT_DOUBLE 3
#define SYSCALL_PRINT_STRING 4

#define SYSCALL_READ_INTEGER 5
#define SYSCALL_READ_FLOAT 6
#define SYSCALL_READ_DOUBLE 7
#define SYSCALL_READ_STRING 8

#define SYSCALL_PRINT_CHARACTER 11
#define SYSCALL_READ_CHARACTER 12

#define SYSCALL_EXIT 10
#define SYSCALL_EXIT2 17

typedef enum {
	NOP = 0,
	ADD, ADDU, ADDI, ADDIU,
	SUB, SUBU, SUBI, SUBIU,
	MOVE,
	SYSCALL,
	NUM_OPCODES
} opcode_t;

typedef enum {
	$zero = 0,
	$at,
	$v0, $v1,
	$a0, $a1, $a2, $a3,
	$t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7,
	$s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7,
	$t8, $t9,
	$k0, $k1,
	$gp, $sp, $fp, $ra,
	NUM_REGISTERS
} register_names;

typedef enum {
	$0 = 0,
	$1,
	$2, $3,
	$4, $5, $6, $7,
	$8, $9, $10, $11, $12, $13, $14, $15,
	$16, $17, $18, $19, $20, $21, $22, $23,
	$24, $25,
	$26, $27,
	$28, $29, $30, $31,
	NUM_REGISTERS_SIMPLE
} register_names_simple;

typedef struct {
	unsigned int registers[32];
	unsigned int pc;
	unsigned int hi;
	unsigned int lo;
	unsigned char memory[4096];
	bool should_exit;
} vm_t;

typedef struct {
	unsigned char buffer[4]; // opcode, rdest, rorg1, rorg2
} instruction_t;

int vm_get_register_i32(vm_t *vm, size_t idx)
{
	return (int)vm->registers[idx];
}

unsigned int vm_get_register_u32(vm_t *vm, size_t idx)
{
	return (unsigned int)vm->registers[idx];
}

void vm_set_register_i32(vm_t *vm, size_t idx, int value)
{
	*((int*)(&(vm->registers[idx]))) = value;
}

void vm_set_register_u32(vm_t *vm, size_t idx, unsigned int value)
{
	*((unsigned int*)(&(vm->registers[idx]))) = value;
}

#define vm_register_set(type, vm, idx, value) (*((type*)(&(vm->registers[(size_t)(idx)]))) = value)
#define vm_register_get(type, vm, idx) (*((type*)(&(vm->registers[(size_t)(idx)]))))

void vm_execute_syscall(vm_t *vm)
{
	switch(vm->registers[$v0])
	{
		case SYSCALL_PRINT_INTEGER:
		{
			printf("%d", vm_register_get(int, vm, $a0));
		}
		break;
		case SYSCALL_PRINT_FLOAT:
		{
			printf("%f", vm_register_get(float, vm, $a0));
		}
		break;
		case SYSCALL_PRINT_STRING:
		{
			printf("%s", vm->memory[vm->registers[$a0]]);
		}
		break;
		case SYSCALL_READ_INTEGER:
		{
			int temp;
			scanf("%d", &temp);
			vm->registers[$v0] = temp;
		}
		break;
		case SYSCALL_READ_FLOAT:
		{
			float temp;
			scanf("%f", &temp);
			(*(float*)(&(vm->registers[$v0]))) = temp;
		}
		case SYSCALL_READ_STRING:
		{
			// TODO: Add a check so that out of bounds memory writes would not crash the actual machine but rather throw some kind of "exception" or error for the program... so that we can keep running without the universe exploding...
			char *buffer = (char *)&(vm->memory[vm->registers[$a0]]);
			unsigned int buffer_size = (unsigned int)vm->registers[$a1];
			scanf("%*s", buffer_size, buffer);
		}
		break;
		case SYSCALL_PRINT_CHARACTER:
		{
			printf("%c", (char)vm->registers[$a0]);
		}
		break;
		case SYSCALL_READ_CHARACTER:
		{
			char c;
			scanf("%c", &c);
			vm->registers[$v0] = c;
		}
		break;
		case SYSCALL_EXIT:
		{
			printf("\nProgram exited normally\n");
			vm->should_exit = true;
		}
		break;
		case SYSCALL_EXIT2:
		{
			int exit_code = vm->registers[$a0];
			vm->should_exit = true;
			printf("\nProgram exited with code %d\n", exit_code);
		}
		break;
	}
}

void vm_execute_instruction(vm_t *vm, instruction_t instruction)
{
	switch(instruction.buffer[0])
	{
		case NOP:
			break;
		case ADD:
			vm_set_register_i32(vm, instruction.buffer[1], vm_get_register_i32(vm, instruction.buffer[2]) + vm_get_register_i32(vm, instruction.buffer[3]));
			break;
		case ADDU:
			vm_set_register_u32(vm, instruction.buffer[1], vm_get_register_u32(vm, instruction.buffer[2]) + vm_get_register_u32(vm, instruction.buffer[3]));
			break;
		case ADDI:
			vm_set_register_i32(vm, instruction.buffer[1], vm_get_register_i32(vm, instruction.buffer[2]) + (int)instruction.buffer[3]);
			break;
		case ADDIU:
			vm_set_register_u32(vm, instruction.buffer[1], vm_get_register_u32(vm, instruction.buffer[2]) + (unsigned int)instruction.buffer[3]);
			break;
		case SUB:
			vm_set_register_i32(vm, instruction.buffer[1], vm_get_register_i32(vm, instruction.buffer[2]) - vm_get_register_i32(vm, instruction.buffer[3]));
			break;
		case SUBU:
			vm_set_register_u32(vm, instruction.buffer[1], vm_get_register_u32(vm, instruction.buffer[2]) - vm_get_register_u32(vm, instruction.buffer[3]));
			break;
		case SUBI:
			vm_set_register_i32(vm, instruction.buffer[1], vm_get_register_i32(vm, instruction.buffer[2]) - (int)instruction.buffer[3]);
			break;
		case SUBIU:
			vm_set_register_u32(vm, instruction.buffer[1], vm_get_register_u32(vm, instruction.buffer[2]) - (unsigned int)instruction.buffer[3]);
			break;
		case MOVE:
			vm->registers[instruction.buffer[1]] = vm->registers[instruction.buffer[2]];
			break;
		case SYSCALL:
			vm_execute_syscall(vm);
			break;
	}
	vm->pc += 1;
	vm->registers[0] = 0;
}

void vm_execute_instructions(vm_t *vm, instruction_t *instructions, size_t count)
{
	vm->should_exit = false;
	vm->pc = 0;
	while(!vm->should_exit)
	{
		vm_execute_instruction(vm, instructions[vm->pc]);
		if(vm->pc >= count && !vm->should_exit)
		{
			printf("\nProgram dropped off bottom\n");
			vm->should_exit = true;
		}
	}
}

int main()
{
	vm_t vm;
	instruction_t instructions[] = {
		{MOVE, $t0, $zero},
		{MOVE, $t1, $zero},
		{ADDI, $t0, $zero, 60},
		{ADDI, $t1, $zero, 9},
		{ADD, $t2, $t1, $t0},
		{ADDI, $v0, $zero, SYSCALL_PRINT_INTEGER},
		{MOVE, $a0, $t2},
		{SYSCALL},
		{ADDI, $v0, $zero, SYSCALL_PRINT_CHARACTER},
		{ADDI, $a0, $zero, '!'},
		{SYSCALL},
		{ADDI, $a0, $zero, 101},
		{ADDI, $v0, $zero, SYSCALL_EXIT2},
		{SYSCALL},
	};
	size_t count = sizeof(instructions) / sizeof(instructions[0]);
	vm_execute_instructions(&vm, instructions, count);
	return 0;
}
