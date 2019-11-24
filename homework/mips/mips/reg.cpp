#include "reg.h"

namespace reg {

	string RegToString(Reg reg) {
		switch (reg) {
		case Reg::zero:
			return "$zero";
		case Reg::at:
			return "$at";
		case Reg::v0:
			return "$v0";
		case Reg::v1:
			return "$v1";
		case Reg::a0:
			return "$a0";
		case Reg::a1:
			return "$a1";
		case Reg::a2:
			return "$a2";
		case Reg::a3:
			return "$a3";
		case Reg::s0:
			return "$s0";
		case Reg::s1:
			return "$s1";
		case Reg::s2:
			return "$s2";
		case Reg::s3:
			return "$s3";
		case Reg::s4:
			return "$s4";
		case Reg::s5:
			return "$s5";
		case Reg::s6:
			return "$s6";
		case Reg::s7:
			return "$s7";
		case Reg::t0:
			return "$t0";
		case Reg::t1:
			return "$t1";
		case Reg::t2:
			return "$t2";
		case Reg::t3:
			return "$t3";
		case Reg::t4:
			return "$t4";
		case Reg::t5:
			return "$t5";
		case Reg::t6:
			return "$t6";
		case Reg::t7:
			return "$t7";
		case Reg::t8:
			return "$t8";
		case Reg::t9:
			return "$t9";
		case Reg::k0:
			return "$k0";
		case Reg::k1:
			return "$k1";
		case Reg::gp:
			return "$gp";
		case Reg::sp:
			return "$sp";
		case Reg::fp:
			return "$fp";
		case Reg::ra:
			return "$ra";
		default:
			assert(0);
		}
		return "wrong";
	}

	Reg NumberToReg(int regNumber) {
		switch (regNumber) {
		case 0:
			return Reg::zero;
		case 1:
			return Reg::at;
		case 2:
			return Reg::v0;
		case 3:
			return Reg::v1;
		case 4:
			return Reg::a0;
		case 5:
			return Reg::a1;
		case 6:
			return Reg::a2;
		case 7:
			return Reg::a3;
		case 8:
			return Reg::t0;
		case 9:
			return Reg::t1;
		case 10:
			return Reg::t2;
		case 11:
			return Reg::t3;
		case 12:
			return Reg::t4;
		case 13:
			return Reg::t5;
		case 14:
			return Reg::t6;
		case 15:
			return Reg::t7;
		case 16:
			return Reg::s0;
		case 17:
			return Reg::s1;
		case 18:
			return Reg::s2;
		case 19:
			return Reg::s3;
		case 20:
			return Reg::s4;
		case 21:
			return Reg::s5;
		case 22:
			return Reg::s6;
		case 23:
			return Reg::s7;
		case 24:
			return Reg::t8;
		case 25:
			return Reg::t9;
		case 26:
			return Reg::k0;
		case 27:
			return Reg::k1;
		case 28:
			return Reg::gp;
		case 29:
			return Reg::sp;
		case 30:
			return Reg::fp;
		case 31:
			return Reg::ra;
		default:
			assert(0);
		}
		return Reg::wrong;
	}

}