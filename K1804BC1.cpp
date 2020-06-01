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
	// ... TODO: Flags
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
	// ... TODO: Flags
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
	// ... TODO: Flags
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
	// ... TODO: Flags
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
	// ... TODO: Flags
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
	// ... TODO: Flags
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
	// ... TODO: Flags
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
	// ... TODO: Flags
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

void K1804BC1::load(const CommandFields* cmd, ALUReasult* res, ILogger* log) {
	switch (cmd->To) {
	// 000: PQ=F
	case 0:
		_reg_q = res->Y & 0b1111;
		log->log("Load: PQ=F=" + std::to_string(_reg_q));
		break;
	// 001: Нет загрузки
	case 1:
		log->log("Load: -");
		break;
	// 010: Y=POH(A), POH(B)=F
	case 2:
		_regs[cmd->B] = res->Y & 0b1111;
		res->Y = _regs[cmd->A];
		break;
	// Я доделаю(moguchev)
	/*case 3:
		_regs[b] = result & 0b1111;
		result = _regs[a];
		logmessage += "to = B, Y = A;\n";
		break;

	case 4:
		_reg_q = _reg_q >> 1;
	case 5:
		logmessage += "to = B >> 1;\n";
		_regs[b] = (result >> 1) & 0b1111;
		break;

	case 6:
		_reg_q = _reg_q << 1;
	case 7:
		logmessage += "to = B << 1;\n";
		_regs[b] = (result << 1) & 0b1111;
		break;*/
	}
	_reg_y = res->Y & 0b1111;
}

VOID K1804BC1::simulate(ABSTIME time, DSIMMODES mode) {
	auto log = new Logger();

	if (isPosedge(_pin_T)) {
		auto cmd = getCommand();
		log->log("CMD #" + std::to_string(_dbg_counter++));
		
		auto ops = getOperands(cmd, log);
		auto result = ALU(cmd->C0, cmd->Alu, ops, log);
		
		//_reg_z = !(result & 0b1111);
		//_reg_f3 = result & 0b1000;
		//_reg_c4 = result & 0b10000;
		//_reg_ovr = _reg_c4 ^ _reg_f3;

		load(cmd, result, log);

		_inst->log((CHAR*)log->getLog().c_str());

		delete result;
		delete ops;
		delete cmd;
	}

	if (isHigh(_pin_OE)) {
		for (size_t i = 0; i < 4; ++i)
			setState(time, _pin_Y[i], _reg_y & (1 << i));
		setState(time, _pin_Z, _reg_z);
		setState(time, _pin_F3, _reg_f3);
		setState(time, _pin_C4, _reg_c4);
		setState(time, _pin_OVR, _reg_ovr);
	} else {
		for (size_t i = 0; i < 4; ++i)
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
