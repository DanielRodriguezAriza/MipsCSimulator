#ifndef DRA_MIPS_C_VIRTUAL_MACHINE_H
#define DRA_MIPS_C_VIRTUAL_MACHINE_H

// TODO : Add checks for out of bounds memory accesses in case that the input ASM file contains a $-1 or some $9999 impossible to access register, tho maybe that should be performed at the compilation step, we'll see...

// TODO : Solve issue where the union we use for the floating point coprocessor doesn't really allow us to index into the 16 double registers the way we would on MIPS... vm.co1.s[2] corresponds to vm.co1.d[1], and that is correct in C, but from the point of view of MIPS we would still address it through $f2. This means that our addressing is going to give us problems, since the input values will reference $f2, whose internal value is 2, which would actually be pointing to vm.co1.d[2]... this can be shittily fixed by modifying all of the double functions to reference the co1 data through the float type with co1.s, and then type pune it through pointers rather than the union... but that sort of makes our co1 union kinda useless... also, need to figure out what happens when we operate over non aligned register boundaries when using doubles...

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
	bool debug_enabled;
} vm_t;

#define vm_debug_log(vm, fmt, ...) do{if((vm)->debug_enabled){printf("[VM]: "); printf(fmt, ##__VA_ARGS__); printf("\n");}}while(0)

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

void vm_add(vm_t *vm, int dst, int org1, int org2)
{
	vm->registers.i[dst] = vm->registers.i[org1] + vm->registers.i[org2];
}

void vm_addu(vm_t *vm, int dst, int org1, int org2)
{
	vm->registers.u[dst] = vm->registers.u[org1] + vm->registers.u[org2];
}

void vm_addi(vm_t *vm, int dst, int org1, int imm)
{
	vm->registers.i[dst] = vm->registers.i[org1] + imm;
}

void vm_addiu(vm_t *vm, int dst, int org1, unsigned int imm)
{
	vm->registers.u[dst] = vm->registers.u[org1] + imm;
}

void vm_sub(vm_t *vm, int dst, int org1, int org2)
{
	vm->registers.i[dst] = vm->registers.i[org1] - vm->registers.i[org2];
}

void vm_subu(vm_t *vm, int dst, int org1, int org2)
{
	vm->registers.u[dst] = vm->registers.u[org1] - vm->registers.u[org2];
}

void vm_subi(vm_t *vm, int dst, int org1, int imm)
{
	vm->registers.i[dst] = vm->registers.i[org1] - imm;
}

void vm_subiu(vm_t *vm, int dst, int org1, unsigned int imm)
{
	vm->registers.u[dst] = vm->registers.u[org1] - imm;
}

void vm_adds(vm_t *vm, int dst, int org1, int org2)
{
	vm->co1.s[dst] = vm->co1.s[org1] + vm->co1.s[org2];
}

void vm_addd(vm_t *vm, int dst, int org1, int org2)
{
	dst /= 2;
	org1 /= 2;
	org2 /= 2;
	vm->co1.d[dst] = vm->co1.d[org1] + vm->co1.d[org2];
}

void vm_subs(vm_t *vm, int dst, int org1, int org2)
{
	vm->co1.s[dst] = vm->co1.s[org1] - vm->co1.s[org2];
}

void vm_subd(vm_t *vm, int dst, int org1, int org2)
{
	dst /= 2;
	org1 /= 2;
	org2 /= 2;
	vm->co1.d[dst] = vm->co1.d[org1] - vm->co1.d[org2];
}

void vm_move(vm_t *vm, int dst, int org)
{
	vm->registers.i[dst] = vm->registers.i[org];
}

void vm_movs(vm_t *vm, int dst, int org)
{
	vm->co1.s[dst] = vm->co1.s[org];
}

void vm_movd(vm_t *vm, int dst, int org)
{
	// Convert the $fx register indices into double register indices
	dst /= 2;
	org /= 2;
	vm->co1.d[dst] = vm->co1.d[org];
}

void vm_mfhi(vm_t *vm, int dst)
{
	vm->registers.u[dst] = vm->hi;
}

void vm_mflo(vm_t *vm, int dst)
{
	vm->registers.u[dst]= vm->lo;
}

void vm_mult(vm_t *vm, int org1, int org2)
{
	long long int tmp = ((long long int)(org1)) * ((long long int)(org2));
	int lo = (int)(tmp);
	int hi = (int)(tmp>>32);
	vm->hi = hi;
	vm->lo = lo;
}

void vm_multu(vm_t *vm, unsigned int org1, unsigned int org2)
{
	unsigned long long int tmp = ((unsigned long long int)(org1)) * ((unsigned long long int)(org2));
	unsigned int lo = (unsigned int)(tmp);
	unsigned int hi = (unsigned int)(tmp>>32);
	vm->hi = hi;
	vm->lo = lo;
}

void vm_mul(vm_t *vm, int dst, int org1, int org2)
{
	vm_mult(vm, org1, org2);
	vm_mflo(vm, dst);
}

void vm_mulu(vm_t *vm, int dst, int org1, int org2)
{
	vm_multu(vm, org1, org2);
	vm_mflo(vm, dst);
}

void vm_muli(vm_t *vm, int dst, int org1, int imm)
{
	vm->registers.i[$at] = imm;
	vm_mult(vm, org1, $at);
	vm_mflo(vm, dst);
}

void vm_muliu(vm_t *vm, int dst, int org1, unsigned int imm)
{
	vm->registers.u[$at] = imm;
	vm_multu(vm, org1, $at);
	vm_mflo(vm, dst);
}

void vm_li(vm_t *vm, int dst, int imm)
{
	vm->registers.i[dst] = imm;
}

void vm_execute_instruction(vm_t *vm, instruction_t instruction)
{
	switch(instruction.opcode)
	{
		case NOP:
			break;
		case ADD:
			vm_debug_log(vm, "add $%d, $%d, $%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_add(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case ADDU:
			vm_debug_log(vm, "addu $%d, $%d, $%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_addu(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case ADDI:
			vm_debug_log(vm, "addi $%d, $%d, %d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_addi(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case ADDIU:
			vm_debug_log(vm, "addiu $%d, $%d, %u", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_addiu(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case SUB:
			vm_debug_log(vm, "sub $%d, $%d, $%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_sub(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case SUBU:
			vm_debug_log(vm, "subu $%d, $%d, $%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_subu(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case SUBI:
			vm_debug_log(vm, "subi $%d, $%d, %d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_subi(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case SUBIU:
			vm_debug_log(vm, "subiu $%d, $%d, %u", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_subiu(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case ADDS:
			vm_debug_log(vm, "add.s $f%d, $f%d, $f%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_adds(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case ADDD:
			vm_debug_log(vm, "add.d $f%d, $f%d, $f%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_addd(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case SUBS:
			vm_debug_log(vm, "sub.s $f%d, $f%d, $f%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_subs(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case SUBD:
			vm_debug_log(vm, "sub.d $f%d, $f%d, $f%d", instruction.data[0], instruction.data[1], instruction.data[2]);
			vm_subd(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case MOVE:
			vm_move(vm, instruction.data[0], instruction.data[1]);
			break;
		case MOVS:
			vm_movs(vm, instruction.data[0], instruction.data[1]);
			break;
		case MOVD:
			vm_movd(vm, instruction.data[0], instruction.data[1]);
			break;
		case MFHI:
			vm_mfhi(vm, instruction.data[0]);
			break;
		case MFLO:
			vm_mflo(vm, instruction.data[0]);
			break;
		case MULT:
			vm_mult(vm, instruction.data[0], instruction.data[1]);
			break;
		case MULTU:
			vm_multu(vm, instruction.data[0], instruction.data[1]);
			break;
		case MUL:
			vm_mul(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case MULU:
			vm_mulu(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case MULI:
			vm_muli(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case MULIU:
			vm_muliu(vm, instruction.data[0], instruction.data[1], instruction.data[2]);
			break;
		case LI:
			vm_li(vm, instruction.data[0], instruction.data[1]);
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
