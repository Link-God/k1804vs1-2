// MT1804BC1.cpp: определяет экспортированные функции для приложения DLL.
//

#include "pch.h"
#include "K1804BC1.h"
#include "logger.h"

uint8_t K1804BC1::genValue(IDSIMPIN** pins, size_t n, size_t offset) {
	uint8_t res = 0;
	for (size_t i = 0; i < n; ++i)
		res |= isHigh(pins[offset + i]) << i;
	return res;
}
void K1804BC1::computeArithmeticFlags(ALUReasult* res, uint8_t c0, const Operands* ops, uint8_t aluCode) {
	
	switch (aluCode) {
		case 3:
			// ~(P3 * P2 * P1 * P0 + Cn); Сn == C0; P = R + S
			res->C4 = ~((ops->R & 0b1000 | ops->S &0b1000) & (ops->R & 0b0100 | ops->S &0b0100)  & 
						(ops->R & 0b0010 | ops->S &0b0010) & (ops->R & 0b0001 | ops->S &0b0001)) | c0
			// ~(P3 * P2 * P1 * P0 + Cn); Сn == C0; P = R + S
			res->OVR = ~((ops->R & 0b1000 | ops->S &0b1000) & (ops->R & 0b0100 | ops->S &0b0100) & 
						(ops->R & 0b0010 | ops->S &0b0010) & (ops->R & 0b0001 | ops->S &0b0001)) | c0
			break;
		case 4:
			// G3 + G2 + G1 + G0 + Cn; Сn == C0; G = R * S
			res->C4 = ((ops->R & 0b1000 & ops->S &0b1000) | (ops->R & 0b0100 & ops->S &0b0100)   |
					   (ops->R & 0b0010 & ops->S &0b0010) | (ops->R & 0b0001 & ops->S &0b0001))  | c0
			// G3 + G2 + G1 + G0 + Cn; Сn == C0; G = R * S
			res->OVR = ((ops->R & 0b1000 & ops->S &0b1000) | (ops->R & 0b0100 & ops->S &0b0100)  |
					    (ops->R & 0b0010 & ops->S &0b0010) | (ops->R & 0b0001 & ops->S &0b0001)) | c0
			break;
		case 5:
			// Так же, как в 4, но все R заменены на ~R
			res->C4 = ((~ops->R & 0b1000 & ops->S &0b1000) | (~ops->R & 0b0100 & ops->S &0b0100)   |
					   (~ops->R & 0b0010 & ops->S &0b0010) | (~ops->R & 0b0001 & ops->S &0b0001))  | c0

			res->OVR = (~(ops->R & 0b1000 & ops->S &0b1000) | (~ops->R & 0b0100 & ops->S &0b0100)  |
					    (~ops->R & 0b0010 & ops->S &0b0010) | (~ops->R & 0b0001 & ops->S &0b0001)) | c0
			break;
		case 6:
			// Так же, как в 7, но все R заменены на ~R
			res->C4 =  ~((~ops->R & 0b1000 & ops->S &0b1000) | ((~ops->R & 0b1000 | ops->S &0b1000) & (~ops->R & 0b0100 & ops->S &0b0100)) |
						((~ops->R & 0b1000 | ops->S &0b1000) & (~ops->R & 0b0100 | ops->S &0b0100)  & (~ops->R & 0b0010 & ops->S &0b0010)) |
						(((~ops->R & 0b1000 | ops->S &0b1000) & (~ops->R & 0b0100 | ops->S &0b0100)  & (~ops->R & 0b0010 | ops->S &0b0010) & (~ops->R & 0b0001 | ops->S &0b0001)) & 
						((~ops->R & 0b0001 & ops->S &0b0001) | ~c0)))

			res->OVR = (~(~ops->R & 0b0100 | ops->S &0b0100) | 
						~(~ops->R & 0b0100 & ops->S &0b0100) & ~(~ops->R & 0b0010 & ops->S &0b0010) & ~(~ops->R & 0b0001 | ops->S &0b0001) |
						~(~ops->R & 0b0100 & ops->S &0b0100) & ~(~ops->R & 0b0010 & ops->S &0b0010) & ~(~ops->R & 0b0001 & ops->S &0b0001) & c0)
						^
						(~(~ops->R & 0b1000 | ops->S &0b1000) |
						~(~ops->R & 0b1000 & ops->S &0b1000) & ~(~ops->R & 0b0100 | ops->S &0b0100) |
						~(~ops->R & 0b1000 & ops->S &0b1000) & ~(~ops->R & 0b0100 & ops->S &0b0100) & ~(~ops->R & 0b0010 | ops->S &0b0010) |
						~(~ops->R & 0b1000 & ops->S &0b1000) & ~(~ops->R & 0b0100 & ops->S &0b0100) & ~(~ops->R & 0b0010 & ops->S &0b0010) & ~(~ops->R & 0b0001 | ops->S &0b0001) |
						~(~ops->R & 0b1000 & ops->S &0b1000) & ~(~ops->R & 0b0100 & ops->S &0b0100) & ~(~ops->R & 0b0010 & ops->S &0b0010) & ~(~ops->R & 0b0001 & ops->S &0b0001) & c0)
			break:
		case 7:
			// ~(G3 + P3 * G2 + P3 * P2 * G1 + P3 * P2 * P1 * P0 * (G1 + ~Cn)); Сn == C0
			res->C4 = ~((ops->R & 0b1000 & ops->S &0b1000) | ((ops->R & 0b1000 | ops->S &0b1000) & (ops->R & 0b0100 & ops->S &0b0100))  |
						((ops->R & 0b1000 | ops->S &0b1000) & (ops->R & 0b0100 | ops->S &0b0100)  & (ops->R & 0b0010 & ops->S &0b0010)) |
						(((ops->R & 0b1000 | ops->S &0b1000) & (ops->R & 0b0100 | ops->S &0b0100)  & (ops->R & 0b0010 | ops->S &0b0010) & (ops->R & 0b0001 | ops->S &0b0001)) & 
						((ops->R & 0b0001 & ops->S &0b0001) | ~c0)))
			// (~P2 + ~G2 * ~P1 + ~G2 * ~G1 * ~P0 + ~G2 * ~G1 * ~G0 * Cn) ^ 
			// (~P3 + ~G3 * ~P2 + ~G3 * ~G2 * ~P1 + ~G3 * ~G2 * ~G1 * ~P0 + ~G3 * ~G2 * ~G1 * ~G0 * Cn)
			res->OVR = (~(ops->R & 0b0100 | ops->S &0b0100) | 
						~(ops->R & 0b0100 & ops->S &0b0100) & ~(ops->R & 0b0010 & ops->S &0b0010) & ~(ops->R & 0b0001 | ops->S &0b0001) |
						~(ops->R & 0b0100 & ops->S &0b0100) & ~(ops->R & 0b0010 & ops->S &0b0010) & ~(ops->R & 0b0001 & ops->S &0b0001) & c0)
						^
						(~(ops->R & 0b1000 | ops->S &0b1000) |
						~(ops->R & 0b1000 & ops->S &0b1000) & ~(ops->R & 0b0100 | ops->S &0b0100) |
						~(ops->R & 0b1000 & ops->S &0b1000) & ~(ops->R & 0b0100 & ops->S &0b0100) & ~(ops->R & 0b0010 | ops->S &0b0010) |
						~(ops->R & 0b1000 & ops->S &0b1000) & ~(ops->R & 0b0100 & ops->S &0b0100) & ~(ops->R & 0b0010 & ops->S &0b0010) & ~(ops->R & 0b0001 | ops->S &0b0001) |
						~(ops->R & 0b1000 & ops->S &0b1000) & ~(ops->R & 0b0100 & ops->S &0b0100) & ~(ops->R & 0b0010 & ops->S &0b0010) & ~(ops->R & 0b0001 & ops->S &0b0001) & c0)		
			break;
		default:
			res->Z = !(res->Y & 0b1111);
			res->C4 = res->Y & 0b10000;
			res->F3 = res->Y & 0b1000;
			res->OVR = res->C4 ^ res->F3;
	}
}

// линковка выходов схемы в Proteus и в коде
VOID K1804BC1::setup(IINSTANCE* instance, IDSIMCKT* dsimckt) {
	_inst = instance;
	_ckt = dsimckt;

	char buffer[16];
	for (size_t i = 0; i < REGISTER_SIZE; ++i) {
		sprintf_s(buffer, "A%d", i);
		_pin_A[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < REGISTER_SIZE; ++i) {
		sprintf_s(buffer, "B%d", i);
		_pin_B[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < REGISTER_SIZE; ++i) {
		sprintf_s(buffer, "D%d", i);
		_pin_D[i] = _inst->getdsimpin(buffer, true);
	}
	for (size_t i = 0; i < 9; ++i) {
		sprintf_s(buffer, "I%d", i);
		_pin_I[i] = _inst->getdsimpin(buffer, true);
	}

	_pin_T = _inst->getdsimpin((CHAR*)"T", true);
	_pin_OE = _inst->getdsimpin((CHAR*)"OE", true);
	_pin_C0 = _inst->getdsimpin((CHAR*)"C0", true);

	for (size_t i = 0; i < REGISTER_SIZE; ++i) {
		sprintf_s(buffer, "Y%d", i);
		_pin_Y[i] = _inst->getdsimpin(buffer, true);
		setState(0, _pin_Y[i], false);
	}
	// выход флагов
	_pin_C4 = _inst->getdsimpin((CHAR*)"C4", true);
	setState(0, _pin_C4, false);
	_pin_Z = _inst->getdsimpin((CHAR*)"Z", true);
	setState(0, _pin_Z, false);
	_pin_F3 = _inst->getdsimpin((CHAR*)"F3", true);
	setState(0, _pin_F3, false);
	_pin_OVR = _inst->getdsimpin((CHAR*)"OVR", true);
	setState(0, _pin_OVR, false);
}

// получение данных со всех входов
K1804BC1::CommandFields* K1804BC1::getCommand() {
	CommandFields* cmd = new CommandFields();
	cmd->From = genValue(_pin_I, 3, 0);
	cmd->Alu = genValue(_pin_I, 3, 3);
	cmd->To = genValue(_pin_I, 3, 6);
	cmd->A = genValue(_pin_A, REGISTER_SIZE);
	cmd->B = genValue(_pin_B, REGISTER_SIZE);
	cmd->D = genValue(_pin_D, REGISTER_SIZE);
	cmd->C0 = isHigh(_pin_C0);

	return cmd;
}

void K1804BC1::__download__000(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = _regs[cmd->A];
	ops->S = _reg_q;
	if (log != nullptr) {
		log->log("From: R=POH(A=" + std::to_string(cmd->A) + ")="
			+ std::to_string(ops->R) + ", S=PQ=" + std::to_string(ops->S));
	}
}

void K1804BC1::__download__001(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = _regs[cmd->A];
	ops->S = _regs[cmd->B];
	if (log != nullptr) {
		log->log("From: R=POH(A=" + std::to_string(cmd->A) + ")="
			+ std::to_string(ops->R) + ", S=POH(B=" + std::to_string(cmd->B) + ")="
			+ std::to_string(ops->S));
	}
}

void K1804BC1::__download__010(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = 0;
	ops->S = _reg_q;
	if (log != nullptr) {
		log->log("From: R=0, S=PQ=" + std::to_string(ops->S));
	}
}

void K1804BC1::__download__011(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = 0;
	ops->S = _regs[cmd->B];
	if (log != nullptr) {
		log->log("From: R=0, S=POH(B=" + std::to_string(cmd->B) + ")="
			+ std::to_string(ops->S));
	}
}

void K1804BC1::__download__100(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = 0;
	ops->S = _regs[cmd->A];
	if (log != nullptr) {
		log->log("From: R=0, S=POH(A=" + std::to_string(cmd->A) + ")="
			+ std::to_string(ops->S));
	}
}

void K1804BC1::__download__101(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = cmd->D;
	ops->S = _regs[cmd->A];
	if (log != nullptr) {
		log->log("From: R=D=" + std::to_string(cmd->D) + ", S=POH(A="
			+ std::to_string(cmd->A) + ")=" + std::to_string(ops->S));
	}

}

void K1804BC1::__download__110(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = cmd->D;
	ops->S = _reg_q;
	if (log != nullptr) {
		log->log("From: R=D=" + std::to_string(cmd->D) + ", S=PQ="
			+ std::to_string(ops->S));
	}
}

void K1804BC1::__download__111(const CommandFields* cmd, Operands* ops, ILogger* log) {
	if (cmd == nullptr || ops == nullptr) {
		return;
	}
	ops->R = cmd->D;
	ops->S = 0;
	if (log != nullptr) {
		log->log("From: R=D=" + std::to_string(cmd->D) + ", S=0");
	}
}

// Загрузка операндов
K1804BC1::Operands* K1804BC1::getOperands(const CommandFields* cmd, ILogger* log) {
	if (cmd == nullptr) {
		return nullptr;
	}
	Operands* ops = new Operands();
	// I0-I2
	switch (cmd->From) {
		// 000: R=POH(A), S=PQ
	case 0:
		__download__000(cmd, ops, log);
		break;
		// 001: R=POH(A), S=POH(B)
	case 1:
		__download__001(cmd, ops, log);
		break;
		// 010: R=0, S=PQ
	case 2:
		__download__010(cmd, ops, log);
		break;
		// 011: R=0, S=POH(B)
	case 3:
		__download__011(cmd, ops, log);
		break;
		// 100: R=0, S=POH(A)
	case 4:
		__download__100(cmd, ops, log);
		break;
		// 101: R=D, S=POH(A)
	case 5:
		__download__101(cmd, ops, log);
		break;
		// 110: R=D, S=PQ
	case 6:
		__download__110(cmd, ops, log);
		break;
		// 111: R=D, S=0
	case 7:
		__download__111(cmd, ops, log);
		break;
	}
	return ops;
}

void K1804BC1::__alu__000(uint8_t c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R + ops->S + c0;
	if (log != nullptr) {
		log->log("ALU: Y=R+S+C0=" + std::to_string(ops->R) + "+"
			+ std::to_string(ops->S) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 0);
}

void K1804BC1::__alu__001(uint8_t c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->S + (ops->R & 0b1111) + c0;
	if (log != nullptr) {
		log->log("ALU: Y=S-R-1+C0=" + std::to_string(ops->S) + "-"
			+ std::to_string(ops->R) + "-1+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 1);
}

void K1804BC1::__alu__010(uint8_t c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R + (ops->S & 0b1111) + c0;
	if (log != nullptr) {
		log->log("ALU: Y=R-S-1+C0=" + std::to_string(ops->R) + "-"
			+ std::to_string(ops->S) + "-1+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 2);
}

void K1804BC1::__alu__011(const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R | ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=RvS=" + std::to_string(ops->R) + "v"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 3);
}

void K1804BC1::__alu__100(const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R & ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=R&S=" + std::to_string(ops->R) + "&"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 4);
}

void K1804BC1::__alu__101(const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = (~ops->R & 0b1111) & ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=~R&S=" + std::to_string(ops->R) + "&"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 5);
}

void K1804BC1::__alu__110(const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R ^ ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=R^S=" + std::to_string(ops->R) + "^"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 6);
}

void K1804BC1::__alu__111(const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = (~(ops->R ^ ops->S)) & 0b1111;
	if (log != nullptr) {
		log->log("ALU: Y=~(R^S)=~(" + std::to_string(ops->R) + "^"
			+ std::to_string(ops->S) + ")=" + std::to_string(res->Y));
	}
	computeArithmeticFlags(res, c0, ops, 7);
}

K1804BC1::ALUReasult* K1804BC1::ALU(uint8_t c0, uint8_t code, const Operands* ops, ILogger* log) {
	if (ops == nullptr) {
		return nullptr;
	}
	auto res = new ALUReasult();
	switch (code) {
		// 0(1) 000: R+S+C0
	case 0:
		__alu__000(c0, ops, res, log);
		break;
		// 0(1) 001: S-R-1+C0
	case 1:
		__alu__001(c0, ops, res, log);
		break;
		// 0(1) 010: R-S-1+C0
	case 2:
		__alu__010(c0, ops, res, log);
		break;
		// - 011: RvS
	case 3:
		__alu__011(ops, res, log);
		break;
		// - 100: R & S
	case 4:
		__alu__100(ops, res, log);
		break;
		// - 101: ~R & S
	case 5:
		__alu__101(ops, res, log);
		break;
		// - 110: R ^ S
	case 6:
		__alu__110(ops, res, log);
		break;
		// - 111  ~(R ^ S)
	case 7:
		__alu__111(ops, res, log);
		break;
	}
	return res;
}

void K1804BC1::__load__000(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	_reg_q = res->Y & 0b1111;
	if (log != nullptr) {
		log->log("Load: PQ=F=" + std::to_string(_reg_q));
	}
}

void K1804BC1::__load__001(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	if (log != nullptr) {
		log->log("Load: -");
	}
}

void K1804BC1::__load__010(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	_regs[cmd->B] = res->Y & 0b1111;
	res->Y = _regs[cmd->A];
	if (log != nullptr) {
		log->log("Load: Y=POH(A=" + std::to_string(cmd->A) + ")=" +
			std::to_string(res->Y & 0b1111) + ", POH(B=" + std::to_string(cmd->B) + ")=" +
			std::to_string(_regs[cmd->B]));
	}
}

void K1804BC1::__load__011(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	_regs[cmd->B] = res->Y & 0b1111;
	if (log != nullptr) {
		log->log("Load: POH(B=" + std::to_string(cmd->B) + ")=" +
			std::to_string(_regs[cmd->B]));
	}
}

void K1804BC1::__load__100(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	// PR0->X1, PQ0->Y2
		// Выставляем на выходы PR0, PQ0 значения выталкиваемых битов X1, Y2
		//PQ0 = _reg_q & 0b0001;
		//PR0 = res->Y & 0b0001;
	uint8_t pq = _reg_q >> 1;
	uint8_t ron = res->Y >> 1;

	// X2->PR3, Y2->PQ3
	// Получаем с входов PR3, PQ3 значения задвигающих битов X2, Y2
	//if (PR3 == 1) {
	//	ron |= 0b1000;
	//}
	//if (PQ3 == 1) {
	//	pq |= 0b1000;
	//}
	_reg_q = (pq) & 0b1111;
	_regs[cmd->B] = (ron) & 0b1111;
	if (log != nullptr) {
		log->log("PQ=Q/2=" + std::to_string(_reg_q) + ", POH(B=" +
			std::to_string(cmd->B) + ")=F/2=" +
			std::to_string(_regs[cmd->B]));
	}
}

void K1804BC1::__load__101(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	// PR0->X1
		// Выставляем на выход PR0 значение выталкиваемого бита X1
		//PR0 = res->Y & 0b0001;
	uint8_t ron = res->Y >> 1;

	// X2->PR3
	// Получаем с входа PR3 значение задвигающего битов X2
	//if (PR3 == 1) {
	//	ron |= 0b1000;
	//}
	_regs[cmd->B] = (ron) & 0b1111;
	if (log != nullptr) {
		log->log("POH(B=" + std::to_string(cmd->B) + ")=F/2=" +
			std::to_string(_regs[cmd->B]));
	}
}
void K1804BC1::__load__110(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	uint8_t pq = _reg_q << 1;
	uint8_t ron = res->Y << 1;

	// PR0<-X1, PQ0<-Y2
	// Получаем со входов PR0, PQ0 значения задвигающих битов X1, Y2
	//if (PR0 == 1) {
	//	ron |= 0b0001;
	//}
	//if (PQ0 == 1) {
	//	pq |= 0b0001;
	//}

	// X2<-PR3, Y2<-PQ3
	// Выставляем на выходы PR3, PQ3 значения выталкиваемых битов X2, Y2
	//PQ3 = pq & 0b10000;
	//PR3 = ron & 0b10000;

	_reg_q = (pq) & 0b1111;
	_regs[cmd->B] = (ron) & 0b1111;
	if (log != nullptr) {
		log->log("PQ=2Q=" + std::to_string(_reg_q) + ", POH(B=" +
			std::to_string(cmd->B) + ")=2F=" +
			std::to_string(_regs[cmd->B]));
	}
}
void K1804BC1::__load__111(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	if (cmd == nullptr || res == nullptr) {
		return;
	}
	uint8_t ron = res->Y << 1;

	// PR0<-X1
	// Получаем со входа PR0 значение задвигающего бита X1
	//if (PR0 == 1) {
	//	ron |= 0b0001;
	//}
	
	// X2<-PR3
	// Выставляем на выход PR3 значение выталкиваемого бита X2
	//PR3 = ron & 0b10000;
	_regs[cmd->B] = (ron) & 0b1111;
	if (log != nullptr) {
		log->log("POH(B=" + std::to_string(cmd->B) + ")=2F=" +
			std::to_string(_regs[cmd->B]));
	}
}

void K1804BC1::load(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	switch (cmd->To) {
		// 000: PQ=F
	case 0:
		__load__000(cmd, res, log);
		break;
		// 001: Нет загрузки
	case 1:
		__load__001(cmd, res, log);
		break;
		// 010: Y=POH(A), POH(B)=F
	case 2:
		__load__010(cmd, res, log);
		break;
		// 011: POH(B)=F
	case 3:
		__load__011(cmd, res, log);
		break;
		// 100: PQ=Q/2, POH(B)=F/2 (сдвиг вправо)
	case 4:
		__load__100(cmd, res, log);
		break;
		// 101: POH(B)=F/2 (сдвиг вправо)
	case 5:
		__load__101(cmd, res, log);
		break;
		// 110: PQ=2PQ, POH(B)=2F (сдвиг вдево)
	case 6:
		__load__110(cmd, res, log);
		break;
		// 111: POH(B)=2F (сдвиг вдево)
	case 7:
		__load__111(cmd, res, log);
		break;
	}
	// в res->Y лежит всегда нужный выход
	_reg_y = res->Y & 0b1111;
}

VOID K1804BC1::simulate(ABSTIME time, DSIMMODES mode) {
	auto log = new Logger();

	if (isPosedge(_pin_T)) {
		// получение микрокоманды
		auto cmd = getCommand();
		log->log("CMD #" + std::to_string(_dbg_counter++));

		// загрузка оператов
		auto ops = getOperands(cmd, log);
		// вычисление в АЛУ
		auto result = ALU(cmd->C0, cmd->Alu, ops, log);
		// выставление флагов
		_reg_z = result->Z;
		_reg_f3 = result->F3;
		_reg_c4 = result->C4;
		_reg_ovr = result->OVR;
		// загрузка результов
		load(cmd, result, log);
		// логирование
		_inst->log((CHAR*)log->getLog().c_str());

		delete result;
		delete ops;
		delete cmd;
	}

	if (isHigh(_pin_OE)) {
		for (size_t i = 0; i < REGISTER_SIZE; ++i)
			setState(time, _pin_Y[i], _reg_y & (1 << i));
		setState(time, _pin_Z, _reg_z);
		setState(time, _pin_F3, _reg_f3);
		setState(time, _pin_C4, _reg_c4);
		setState(time, _pin_OVR, _reg_ovr);
	}
	else {
		for (size_t i = 0; i < REGISTER_SIZE; ++i)
			setState(time, _pin_Y[i], false);
		setState(time, _pin_Z, false);
		setState(time, _pin_F3, false);
		setState(time, _pin_C4, false);
		setState(time, _pin_OVR, false);
	}

	delete log;
}

bool K1804BC1::isHigh(IDSIMPIN* pin) {
	return ishigh(pin->istate());
}

bool K1804BC1::isLow(IDSIMPIN* pin) {
	return islow(pin->istate());
}

bool K1804BC1::isNegedge(IDSIMPIN* pin) {
	return pin->isnegedge();
}

bool K1804BC1::isPosedge(IDSIMPIN* pin) {
	return pin->isposedge();
}

void K1804BC1::setState(ABSTIME time, IDSIMPIN* pin, bool set) {
	pin->setstate(time, 1, set ? SHI : SLO);
}

INT K1804BC1::isdigital(CHAR* pinname) {
	return 1;
}

VOID K1804BC1::runctrl(RUNMODES mode) {
}

VOID K1804BC1::actuate(REALTIME time, ACTIVESTATE newstate) {
}

VOID K1804BC1::callback(ABSTIME time, EVENTID eventid) {
}

BOOL K1804BC1::indicate(REALTIME time, ACTIVEDATA* data) {
	return FALSE;
}
