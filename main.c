#include "mips.h"

int main()
{
	vm_t vm;
	vm.debug_enabled = true;
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
