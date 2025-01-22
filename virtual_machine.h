#ifndef DRA_MIPS_C_VIRTUAL_MACHINE_H
#define DRA_MIPS_C_VIRTUAL_MACHINE_H

// TODO : Add checks for out of bounds memory accesses in case that the input ASM file contains a $-1 or some $9999 impossible to access register, tho maybe that should be performed at the compilation step, we'll see...

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "register.h"
#include "syscall.h"
#include "instruction.h"

#define vm_register_set(type, vm, idx, value) (*((type*)(&(vm->registers[(size_t)(idx)]))) = value)
#define vm_register_get(type, vm, idx) (*((type*)(&(vm->registers[(size_t)(idx)]))))

typedef struct {
	union {
		int i[32];
		unsigned int u[32];
	} registers;
	union {
		float s[32];
		double d[16];
	} co1;
	unsigned int pc;
	unsigned int hi;
	unsigned int lo;
	unsigned char memory[4096]; // TODO : Maybe 4 GB in size (full range of a 32 bit address space) and also make it so that the instructions go inside of this rather than as a separate pointer... that would allow self modifying programs to exist in the future, too!
	bool should_exit; // bruh...
} vm_t;

void vm_execute_syscall(vm_t *vm)
{
	switch(vm->registers.i[$v0])
	{
		case SYSCALL_PRINT_INTEGER:
		{
			printf("%d", vm->registers.i[$a0]);
		}
		break;
		case SYSCALL_PRINT_FLOAT:
		{
			// printf("%f", vm_register_get(float, vm, $a0)); // TODO : Implement. Read from correct co1 register for float printing.
		}
		break;
		case SYSCALL_PRINT_STRING:
		{
			printf("%s", (char*)&(vm->memory[vm->registers.u[$a0]]));
		}
		break;
		case SYSCALL_READ_INTEGER:
		{
			int temp;
			scanf("%d", &temp);
			vm->registers.i[$v0] = temp;
		}
		break;
		case SYSCALL_READ_FLOAT:
		{
			// TODO : Implement, etc...
			// float temp;
			// scanf("%f", &temp);
			// (*(float*)(&(vm->registers[$v0]))) = temp;
		}
		case SYSCALL_READ_STRING:
		{
			// TODO: Add a check so that out of bounds memory writes would not crash the actual machine but rather throw some kind of "exception" or error for the program... so that we can keep running without the universe exploding...
			// TODO : Reimplement
			// char *buffer = (char *)&(vm->memory[vm->registers[$a0]]);
			// unsigned int buffer_size = (unsigned int)vm->registers[$a1];
			// scanf("%*s", buffer_size, buffer);
		}
		break;
		case SYSCALL_PRINT_CHARACTER:
		{
			printf("%c", (char)vm->registers.i[$a0]);
		}
		break;
		case SYSCALL_READ_CHARACTER:
		{
			char c;
			scanf("%c", &c);
			vm->registers.i[$v0] = c;
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
			int exit_code = vm->registers.i[$a0];
			vm->should_exit = true;
			printf("\nProgram exited with code %d\n", exit_code);
		}
		break;
	}
}

void vm_add_i32(vm_t *vm, int dst, int a, int b)
{
	vm->registers.i[dst] = a + b;
}

void vm_add_u32(vm_t *vm, int dst, unsigned int a, unsigned int b)
{
	vm->registers.u[dst]= a + b;
}

void vm_sub_i32(vm_t *vm, int dst, int a, int b)
{
	vm->registers.i[dst] = a - b;
}

void vm_sub_u32(vm_t *vm, int dst, unsigned int a, unsigned int b)
{
	vm->registers.u[dst] = a - b;
}

void vm_move_i(vm_t *vm, int dst, int org)
{
	vm->registers.i[dst] = vm->registers.i[org];
}

void vm_move_s(vm_t *vm, int dst, int org)
{
	vm->co1.s[dst] = vm->co1.s[org];
}

void vm_move_d(vm_t *vm, int dst, int org)
{
	vm->co1.d[dst] = vm->co1.d[org];
}

void vm_execute_instruction(vm_t *vm, instruction_t instruction)
{
	switch(instruction.buffer[0])
	{
		case NOP:
			break;
		case ADD:
			vm_add_i32(vm, instruction.buffer[1], vm->registers.i[instruction.buffer[2]], vm->registers.i[instruction.buffer[3]]);
			break;
		case ADDU:
			vm_add_u32(vm, instruction.buffer[1], vm->registers.u[instruction.buffer[2]], vm->registers.u[instruction.buffer[3]]);
			break;
		case ADDI:
			vm_add_i32(vm, instruction.buffer[1], vm->registers.i[instruction.buffer[2]], (int)instruction.buffer[3]);
			break;
		case ADDIU:
			vm_add_u32(vm, instruction.buffer[1], vm->registers.u[instruction.buffer[2]], (unsigned int)instruction.buffer[3]);
			break;
		case SUB:
			vm_sub_i32(vm, instruction.buffer[1], vm->registers.i[instruction.buffer[2]], vm->registers.i[instruction.buffer[3]]);
			break;
		case SUBU:
			vm_sub_u32(vm, instruction.buffer[1], vm->registers.u[instruction.buffer[2]], vm->registers.u[instruction.buffer[3]]);
			break;
		case SUBI:
			vm_sub_i32(vm, instruction.buffer[1], vm->registers.i[instruction.buffer[2]], (int)instruction.buffer[3]);
			break;
		case SUBIU:
			vm_sub_u32(vm, instruction.buffer[1], vm->registers.u[instruction.buffer[2]], (unsigned int)instruction.buffer[3]);
			break;
		case MOVE:
			vm_move_i(vm, instruction.buffer[1], instruction.buffer[2]);
			break;
		case MOVS:
			vm_move_s(vm, instruction.buffer[1], instruction.buffer[2]);
			break;
		case MOVD:
			vm_move_d(vm, instruction.buffer[1], instruction.buffer[2]);
			break;
		case SYSCALL:
			vm_execute_syscall(vm);
			break;
	}
	vm->pc += 1;
	vm->registers.i[0] = 0;
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

#endif
