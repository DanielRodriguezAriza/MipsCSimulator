#include "mips.h"

int main()
{
	vm_t vm;
	vm.debug_enabled = false;
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
		{ADDI, $a0, $zero, '\n'},
		{SYSCALL},
		{LI, $t0, 10},
		{LI, $t1, 0},
		{LI, $v0, SYSCALL_PRINT_INTEGER},
		{MOVE, $a0, $t1},
		{SYSCALL},
		{LI, $v0, SYSCALL_PRINT_CHARACTER},
		{LI, $a0, '\n'},
		{SYSCALL},
		{ADDI, $t1, $t1, 1},
		{BNE, $t0, $t1, 12},
		{ADDI, $a0, $zero, 101},
		{ADDI, $v0, $zero, SYSCALL_EXIT2},
		{SYSCALL},
	};
	size_t count = sizeof(instructions) / sizeof(instructions[0]);
	vm_execute_instructions(&vm, instructions, count);
	return 0;
}
