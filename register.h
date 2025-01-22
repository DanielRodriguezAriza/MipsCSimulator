#ifndef DRA_MIPS_C_REGISTER_H
#define DRA_MIPS_C_REGISTER_H

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

typedef enum {
	$f0 = 0,
	$f1,
	$f2,
	$f3,
	$f4,
	$f5,
	$f6,
	$f7,
	$f8,
	$f9,
	$f10,
	$f11,
	$f12,
	$f13,
	$f14,
	$f15,
	$f16,
	$f17,
	$f18,
	$f19,
	$f20,
	$f21,
	$f22,
	$f23,
	$f24,
	$f25,
	$f26,
	$f27,
	$f28,
	$f29,
	$f30,
	$f31,
	NUM_REGISTERS_SIMPLE
} register_names_float;

typedef struct {
	unsigned int registers[32];
} register_bank_integer;

typedef struct {
	float registers[32]; // for double precission in MIPS32, we use register pairing, so $f0:$f1 is the pair used for the first double precission floating point "register", which consumes 2 single precission floating point registers. Not sure how real MIPS handles pairing if I pick an odd register number rather than pairing from even numbers, I'll figure this reading some documentation later I suppose.
} register_bank_float;

typedef struct {
	double registers[16]; // we cast to this type later on through either type punning or unions, we'll see.
} register_bank_double;

#endif
