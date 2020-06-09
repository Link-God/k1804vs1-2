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

void K1804BC1::computeFlags(ALUReasult* res, bool c0, const Operands* ops, uint8_t aluCode) {
	bool P0 = (ops->R & 0b0001 || ops->S & 0b0001);
	bool P1 = (ops->R & 0b0010 || ops->S & 0b0010);
	bool P2 = (ops->R & 0b0100 || ops->S & 0b0100);
	bool P3 = (ops->R & 0b1000 || ops->S & 0b1000);

	bool notR_P0 = (~ops->R & 0b0001 || ops->S & 0b0001);
	bool notR_P1 = (~ops->R & 0b0010 || ops->S & 0b0010);
	bool notR_P2 = (~ops->R & 0b0100 || ops->S & 0b0100);
	bool notR_P3 = (~ops->R & 0b1000 || ops->S & 0b1000);

	bool notS_P0 = (ops->R & 0b0001 || ~ops->S & 0b0001);
	bool notS_P1 = (ops->R & 0b0010 || ~ops->S & 0b0010);
	bool notS_P2 = (ops->R & 0b0100 || ~ops->S & 0b0100);
	bool notS_P3 = (ops->R & 0b1000 || ~ops->S & 0b1000);
	
	bool G0 = (ops->R & 0b0001 && ops->S & 0b0001);
	bool G1 = (ops->R & 0b0010 && ops->S & 0b0010);
	bool G2 = (ops->R & 0b0100 && ops->S & 0b0100);
	bool G3 = (ops->R & 0b1000 && ops->S & 0b1000);

	bool notR_G0 = (~ops->R & 0b0001 && ops->S & 0b0001);
	bool notR_G1 = (~ops->R & 0b0010 && ops->S & 0b0010);
	bool notR_G2 = (~ops->R & 0b0100 && ops->S & 0b0100);
	bool notR_G3 = (~ops->R & 0b1000 && ops->S & 0b1000);

	bool notS_G0 = (ops->R & 0b0001 && ~ops->S & 0b0001);
	bool notS_G1 = (ops->R & 0b0010 && ~ops->S & 0b0010);
	bool notS_G2 = (ops->R & 0b0100 && ~ops->S & 0b0100);
	bool notS_G3 = (ops->R & 0b1000 && ~ops->S & 0b1000);
	
	bool C4 = G3 || P3 && G2 || P3 && P2 && G1 || P3 && P2 && P1 && G0 || P3 && P2 && P1 && P0 && c0;
	bool C3 = G2 || P2 && G1 || P2 && P1 && G0 || P2 && P1 && P0 && c0;

	bool notR_C4 = notR_G3 ||
		notR_P3 && G2 ||
		notR_P3 && notR_P2 && notR_G1 ||
		notR_P3 && notR_P2 && notR_P1 && notR_G0 ||
		notR_P3 && notR_P2 && notR_P1 && notR_P0 && c0;
	bool notR_C3 = notR_G2 ||
		notR_P2 && notR_G1 ||
		notR_P2 && notR_P1 && notR_G0 ||
		notR_P2 && notR_P1 && notR_P0 && c0;

	bool notS_C4 = notS_G3 ||
		notS_P3 && notS_G2 || 
		notS_P3 && notS_P2 && notS_G1 || 
		notS_P3 && notS_P2 && notS_P1 && notS_G0 || 
		notS_P3 && notS_P2 && notS_P1 && notS_P0 && c0;
	bool notS_C3 = notS_G2 || 
		notS_P2 && notS_G1 || 
		notS_P2 && notS_P1 && notS_G0 || 
		notS_P2 && notS_P1 && notS_P0 && c0;
	
	res->Z = !(res->Y & 0b1111);
	res->F3 = res->Y & 0b1000;
	
	switch (aluCode) {
	case 0:
		res->P_ = !(P3 && P2 && P1 && P0);
		res->G_ = !(G3 || P3 && G2 || P3 && P2 && G1 || P3 && P2 && P1 && G0);
		res->C4 = C4;
		res->OVR = C3 != C4;
		break;
	case 1:
		res->P_ = !(notR_P3 && notR_P2 && notR_P1 && notR_P0);
		res->G_ = !(notR_G3 || notR_P3 && notR_G2 || notR_P3 && notR_P2 && notR_G1 ||
			notR_P3 && notR_P2 && notR_P1 && notR_G0);
		res->C4 = notR_C4;
		res->OVR = notR_C3 != notR_C4;
		break;
	case 2:
		res->P_ = !(notS_P3 && notS_P2 && notS_P1 && notS_P0);
		res->G_ = !(notS_G3 || notS_P3 && notS_G2 || notS_P3 && notS_P2 && notS_G1 ||
			notS_P3 && notS_P2 && notS_P1 && notS_G0);
		res->C4 = notS_C4;
		res->OVR = notS_C3 != notS_C4;
		break;
	case 3:
		res->P_ = false;
		res->G_ = P3 && P2 && P1 && P0;
		// ~(P3 * P2 * P1 * P0) + Cn; Сn == C0; P = R + S
		res->C4 = !(P3 && P2 && P1 && P0) || c0;
		// ~(P3 * P2 * P1 * P0 + Cn); Сn == C0; P = R + S
		res->OVR = res->C4;
		break;
	case 4:
		res->P_ = false;
		res->G_ = !(G3 || G2 || G1 || G0);
		// G3 + G2 + G1 + G0 + Cn; Сn == C0; G = R * S
		res->C4 = G3 || G2 || G1 || G0 || c0;
		// G3 + G2 + G1 + G0 + Cn; Сn == C0; G = R * S
		res->OVR = res->C4;
		break;
	case 5:
		res->P_ = false;
		res->G_ = !(notR_G3 || notR_G2 || notR_G1 || notR_G0);
		// Так же, как в 4, но все R заменены на ~R
		res->C4 = notR_G3 || notR_G2 || notR_G1 || notR_G0 || c0;
		res->OVR = res->C4;
		
		break;
	case 6:
		res->P_ = notR_G3 || notR_G2 || notR_G1 || notR_G0;
		res->G_ = notR_G3 || notR_P3 && notR_G2 || notR_P3 && notR_P2 && notR_G1 ||
			notR_P3 && notR_P2 && notR_P1 && notR_P0;
		// Так же, как в 7, но все R заменены на ~R

		res->C4 = !(notR_G3 || notR_P3 && notR_G2 || notR_P3 && notR_P2 && notR_G1 ||
			notR_P3 && notR_P2 && notR_P1 && notR_P0 && (notR_G1 || !c0));
		
		res->OVR = (!notR_P2 || !notR_G2 && !notR_P1 || !notR_G2 && !notR_G1 && !notR_P0 ||
					!notR_G2 && !notR_G1 && !notR_G0 && c0)
		!=
					(!notR_P3 || !notR_G3 && !notR_P2 || !notR_G3 && !notR_G2 && !notR_P1 ||
					!notR_G3 && !notR_G2 && !notR_G1 && !notR_P0 || !notR_G3 && !notR_G2 && !notR_G1 && !notR_G0 && c0);
		break;
	case 7:
		res->P_ = G3 || G2 || G1 || G0;
		res->G_ = G3 || P3 && G2 || P3 && P2 && G1 || P3 && P2 && P1 && P0;
		// ~(G3 + P3 * G2 + P3 * P2 * G1 + P3 * P2 * P1 * P0 * (G1 + ~Cn)); Сn == C0
		res->C4 = !(G3 || P3 && G2 || P3 && P2 && G1 || P3 && P2 && P1 && P0 && (G1 || !c0));
		
		
		 		
		// (~P2 + ~G2 * ~P1 + ~G2 * ~G1 * ~P0 + ~G2 * ~G1 * ~G0 * Cn) ^ 
		// (~P3 + ~G3 * ~P2 + ~G3 * ~G2 * ~P1 + ~G3 * ~G2 * ~G1 * ~P0 + ~G3 * ~G2 * ~G1 * ~G0 * Cn)
		res->OVR = (!P2 || !G2 && !P1 || !G2 && !G1 && !P0 || !G2 && !G1 && !G0 && c0) !=
			(!P3 || !G3 && !P2 || !G3 && !G2 && !P1 || !G3 && !G2 && !G1 && !P0 || !G3 && !G2 && !G1 && !G0 && c0);
		break;
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
	for (size_t i = 0; i < PIN_I_SIZE; ++i) {
		sprintf_s(buffer, "I%d", i);
		_pin_I[i] = _inst->getdsimpin(buffer, true);
	}

	_pin_PQ0 = _inst->getdsimpin(const_cast<CHAR*>("PQ0"), true);
	_pin_PQ3 = _inst->getdsimpin(const_cast<CHAR*>("PQ3"), true);
	_pin_PR0 = _inst->getdsimpin(const_cast<CHAR*>("PR0"), true);
	_pin_PR3 = _inst->getdsimpin(const_cast<CHAR*>("PR3"), true);

	_pin_T = _inst->getdsimpin(const_cast<CHAR*>("T"), true);
	_pin_OE = _inst->getdsimpin(const_cast<CHAR*>("OE"), true);
	_pin_C0 = _inst->getdsimpin(const_cast<CHAR*>("C0"), true);

	for (size_t i = 0; i < REGISTER_SIZE; ++i) {
		sprintf_s(buffer, "Y%d", i);
		_pin_Y[i] = _inst->getdsimpin(buffer, true);
		//setState(0, _pin_Y[i], false);
	}
	// выход флагов
	_pin_C4 = _inst->getdsimpin(const_cast<CHAR*>("C4"), true);
	setState(0, _pin_C4, 0);
	_pin_Z = _inst->getdsimpin(const_cast<CHAR*>("Z"), true);
	setState(0, _pin_Z, 0);
	_pin_F3 = _inst->getdsimpin(const_cast<CHAR*>("F3"), true);
	setState(0, _pin_F3, 0);
	_pin_OVR = _inst->getdsimpin(const_cast<CHAR*>("OVR"), true);
	setState(0, _pin_OVR, 0);
	_pin_P = _inst->getdsimpin(const_cast<CHAR*>("P"), true);
	setState(0, _pin_P, 0);
	_pin_G = _inst->getdsimpin(const_cast<CHAR*>("G"), true);
	setState(0, _pin_G, 0);
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

void K1804BC1::__alu__000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R + ops->S + c0;
	if (log != nullptr) {
		log->log("ALU: Y=R+S+C0=" + std::to_string(ops->R) + "+"
			+ std::to_string(ops->S) + "+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 0);
}

void K1804BC1::__alu__001(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->S + (ops->R & 0b1111) + c0;
	if (log != nullptr) {
		log->log("ALU: Y=S-R-1+C0=" + std::to_string(ops->S) + "-"
			+ std::to_string(ops->R) + "-1+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 1);
}

void K1804BC1::__alu__010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R + (ops->S & 0b1111) + c0;
	if (log != nullptr) {
		log->log("ALU: Y=R-S-1+C0=" + std::to_string(ops->R) + "-"
			+ std::to_string(ops->S) + "-1+" + std::to_string(c0) + "="
			+ std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 2);
}

void K1804BC1::__alu__011(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R | ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=RvS=" + std::to_string(ops->R) + "v"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 3);
}

void K1804BC1::__alu__100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R & ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=R&S=" + std::to_string(ops->R) + "&"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 4);
}

void K1804BC1::__alu__101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = (~ops->R & 0b1111) & ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=~R&S=" + std::to_string(ops->R) + "&"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 5);
}

void K1804BC1::__alu__110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = ops->R ^ ops->S;
	if (log != nullptr) {
		log->log("ALU: Y=R^S=" + std::to_string(ops->R) + "^"
			+ std::to_string(ops->S) + "=" + std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 6);
}

void K1804BC1::__alu__111(bool c0, const Operands* ops, ALUReasult* res, ILogger* log) {
	if (ops == nullptr || res == nullptr) {
		return;
	}
	res->Y = (~(ops->R ^ ops->S)) & 0b1111;
	if (log != nullptr) {
		log->log("ALU: Y=~(R^S)=~(" + std::to_string(ops->R) + "^"
			+ std::to_string(ops->S) + ")=" + std::to_string(res->Y));
	}
	computeFlags(res, c0, ops, 7);
}
K1804BC1::ALUReasult* K1804BC1::ALU(bool c0, uint8_t code, const Operands* ops, ILogger* log) {
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
		__alu__011(c0, ops, res, log);
		break;
		// - 100: R & S
	case 4:
		__alu__100(c0, ops, res, log);
		break;
		// - 101: ~R & S
	case 5:
		__alu__101(c0, ops, res, log);
		break;
		// - 110: R ^ S
	case 6:
		__alu__110(c0, ops, res, log);
		break;
		// - 111  ~(R ^ S)
	case 7:
		__alu__111(c0, ops, res, log);
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
	if ((_reg_q & 0b0001)) {
		setState(_time, _pin_PQ0, 1);
	} else {
		setState(_time, _pin_PQ0, -1);
	}

	if (res->Y & 0b0001) {
		setState(_time, _pin_PR0, 1);
	} else {
		setState(_time, _pin_PR0, -1);
	}

	uint8_t pq = _reg_q >> 1;
	uint8_t ron = res->Y >> 1;

	// X2->PR3, Y2->PQ3
	// Получаем с входов PR3, PQ3 значения задвигающих битов X2, Y2
	if (isHigh(_pin_PR3)) {
		ron |= 0b1000;
	}
	if (isHigh(_pin_PQ3)) {
		pq |= 0b1000;
	}
	_reg_q = pq & 0b1111;
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
	if (res->Y & 0b0001) {
		setState(_time, _pin_PR0, 1);
	} else {
		setState(_time, _pin_PR0, -1);
	}
	
	uint8_t ron = res->Y >> 1;

	// X2->PR3
	// Получаем с входа PR3 значение задвигающего битов X2
	if (isHigh(_pin_PR3)) {
		ron |= 0b1000;
	}
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
	if (isHigh(_pin_PR0)) {
		ron |= 0b0001;
	}
	if (isHigh(_pin_PQ0)) {
		pq |= 0b0001;
	}

	// X2<-PR3, Y2<-PQ3
	// Выставляем на выходы PR3, PQ3 значения выталкиваемых битов X2, Y2
	if (pq & 0b10000) {
		setState(_time, _pin_PQ3, 1);
	} else {
		setState(_time, _pin_PQ3, -1);
	}

	if (ron & 0b10000) {
		setState(_time, _pin_PR3, 1);
	} else {
		setState(_time, _pin_PR3, -1);
	}

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
	if (isHigh(_pin_PR0)) {
		ron |= 0b0001;
	}
	
	// X2<-PR3
	// Выставляем на выход PR3 значение выталкиваемого бита X2
	if (ron & 0b10000) {
		setState(_time, _pin_PR3, 1);
	} else {
		setState(_time, _pin_PR3, -1);
	}

	_regs[cmd->B] = (ron) & 0b1111;
	if (log != nullptr) {
		log->log("POH(B=" + std::to_string(cmd->B) + ")=2F=" +
			std::to_string(_regs[cmd->B]));
	}
}

void K1804BC1::load(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	setState(_time, _pin_PR3, 0);
	setState(_time, _pin_PR0, 0);
	setState(_time, _pin_PQ3, 0);
	setState(_time, _pin_PQ0, 0);
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
	_time = time;

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
		_reg_p = result->P_;
		_reg_g = result->G_;
		// загрузка результов
		load(cmd, result, log);
		// логирование
		_inst->log(const_cast<CHAR*>(log->getLog().c_str()));

		delete result;
		delete ops;
		delete cmd;
	}
	// что делать с флагами...
	if (isLow(_pin_OE)) {
		for (size_t i = 0; i < REGISTER_SIZE; ++i)
			setState(time, _pin_Y[i], _reg_y & (1 << i) ? 1 : -1);
		setState(time, _pin_Z, _reg_z ? 1 : -1);
		setState(time, _pin_F3, _reg_f3 ? 1 : -1);
		setState(time, _pin_C4, _reg_c4 ? 1 : -1);
		setState(time, _pin_OVR, _reg_ovr ? 1 : -1);
		setState(time, _pin_P, _reg_p ? 1 : -1);
		setState(time, _pin_G, _reg_g ? 1 : -1);
	}
	else {
		for (size_t i = 0; i < REGISTER_SIZE; ++i)
			setState(time, _pin_Y[i], 0);
		setState(time, _pin_Z, 0);
		setState(time, _pin_F3, 0);
		setState(time, _pin_C4, 0);
		setState(time, _pin_OVR, 0);
		setState(time, _pin_P, 0);
		setState(time, _pin_G, 0);
	}

	delete log;
}

bool K1804BC1::isHigh(IDSIMPIN* pin) {
	// return ishigh(pin->istate());
	return pin->activity() == 1;
}

bool K1804BC1::isLow(IDSIMPIN* pin) {
	//return islow(pin->istate());
	return pin->activity() == -1;
}

bool K1804BC1::isNegedge(IDSIMPIN* pin) {
	return pin->isnegedge();
}

bool K1804BC1::isPosedge(IDSIMPIN* pin) {
	return pin->isposedge();
}

void K1804BC1::setState(ABSTIME time, IDSIMPIN* pin, int set) {
	switch (set)
	{
	case 1:
		pin->setstate(time, 1, SHI);
		break;
	case -1:
		pin->setstate(time, 1, SLO);
		break;
	case 0:
		pin->setstate(time, 1, FLT);
		break;
	}
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
