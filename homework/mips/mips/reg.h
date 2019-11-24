#pragma once

#include<string>
#include <cassert>

using namespace std;

enum class Reg {
	zero,
	at,
	v0, v1,
	a0, a1, a2, a3,
	s0, s1, s2, s3, s4, s5, s6, s7,
	t0, t1, t2, t3, t4, t5, t6, t7,
	t8, t9,
	k0, k1,
	gp,
	sp,
	fp,
	ra,
	wrong
};

namespace reg {
	string RegToString(Reg reg);

	Reg NumberToReg(int regNumber);
}
