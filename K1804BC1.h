#pragma once
#include <windows.h>
#include "pch.h"
#include "vsm.h"
#include <string>
#include <cinttypes>

#include "logger.h"

const int REGISTER_SIZE = 4;
const int NUM_OF_REGISTERS = 16;
const int PIN_I_SIZE = 9;


class K1804BC1 : public IDSIMMODEL
{
	IINSTANCE* _inst;
	IDSIMCKT* _ckt;
	ABSTIME _time;

	uint32_t _dbg_counter;
	uint8_t _regs[NUM_OF_REGISTERS];
	uint8_t _reg_q;
	uint8_t _reg_y;
	bool _reg_z;
	bool _reg_f3;
	bool _reg_c4;
	bool _reg_ovr;
	bool _reg_p;
	bool _reg_g;
	IDSIMPIN* _pin_A[REGISTER_SIZE];
	IDSIMPIN* _pin_B[REGISTER_SIZE];
	IDSIMPIN* _pin_D[REGISTER_SIZE];
	IDSIMPIN* _pin_I[PIN_I_SIZE];
	IDSIMPIN* _pin_OE;
	IDSIMPIN* _pin_T;
	IDSIMPIN* _pin_C0;
	IDSIMPIN* _pin_Y[REGISTER_SIZE];
	IDSIMPIN* _pin_C4;
	IDSIMPIN* _pin_Z;
	IDSIMPIN* _pin_F3;
	IDSIMPIN* _pin_P;
	IDSIMPIN* _pin_G;
	IDSIMPIN* _pin_OVR;
	IDSIMPIN* _pin_PR0;
	IDSIMPIN* _pin_PQ0;
	IDSIMPIN* _pin_PR3;
	IDSIMPIN* _pin_PQ3;
	uint8_t genValue(IDSIMPIN** pins, size_t n, size_t offset = 0);
	bool isHigh(IDSIMPIN* pin);
	bool isLow(IDSIMPIN* pin);
	bool isNegedge(IDSIMPIN* pin);
	bool isPosedge(IDSIMPIN* pin);
	void setState(ABSTIME time, IDSIMPIN* pin, int set);

	struct CommandFields {
		uint8_t From;
		uint8_t Alu;
		uint8_t To;
		uint8_t A;
		uint8_t B;
		uint8_t D;
		bool C0;
	};
	CommandFields* getCommand();

	struct Operands {
		uint8_t R;
		uint8_t S;
	};
	Operands* getOperands(const CommandFields* cmd, ILogger* log);

	void __download__000(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__001(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__010(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__011(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__100(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__101(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__110(const CommandFields* cmd, Operands* ops, ILogger* log);
	void __download__111(const CommandFields* cmd, Operands* ops, ILogger* log);

	struct ALUReasult {
		uint8_t Y; // ��������� ���
		bool OVR; // ������������
		bool C4; // ������� �� �������� �������
		bool F3; // ����, ���������� �������� ������� ���
		bool Z; // ������� �������� ����������
		bool G_; // ������ ��������� �������� �� ���
		bool P_; // ������ ��������������� �������� �� ���
	};

	ALUReasult* ALU(bool c0, uint8_t code, const Operands* ops, ILogger* log);

	void __alu__000(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__001(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__010(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__011(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__100(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__101(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__110(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);
	void __alu__111(bool c0, const Operands* ops, ALUReasult* res, ILogger* log);

	void load(const CommandFields* cmd, ALUReasult* res, ILogger* log);

	void __load__000(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__001(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__010(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__011(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__100(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__101(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__110(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	void __load__111(const CommandFields* cmd, ALUReasult* res, ILogger* log);
	
	void computeFlags(ALUReasult* res, bool c0, const Operands* ops, uint8_t aluCode);
public:
	INT isdigital(CHAR* pinname);
	VOID setup(IINSTANCE* inst, IDSIMCKT* dsim);
	VOID runctrl(RUNMODES mode);
	VOID actuate(REALTIME time, ACTIVESTATE newstate);
	BOOL indicate(REALTIME time, ACTIVEDATA* data);
	VOID simulate(ABSTIME time, DSIMMODES mode);
	VOID callback(ABSTIME time, EVENTID eventid);
};
