#pragma once
#include <cinttypes>

struct Operands {
	uint8_t R;
	uint8_t S;
};

struct ALUReasult {
	uint8_t Y; // Результат Алу
	bool OVR; // Переполнение
	bool C4; // Перенос из старшего разряда
	bool F3; // Знак, содержимое старшего разряда АЛУ
	bool Z; // Признак нулевого результата
	bool G_; // Сигнал генерации переноса из АЛУ
	bool P_; // Сигнал распространения переноса из АЛУ
};

void computeFlags(ALUReasult* res, bool c0, const Operands* ops, uint8_t aluCode) {
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

		// res->C4 = ~((ops->R & 0b1000 | ops->S & 0b1000) & (ops->R & 0b0100 | ops->S & 0b0100) &
		// 	(ops->R & 0b0010 | ops->S & 0b0010) & (ops->R & 0b0001 | ops->S & 0b0001)) | c0;

		// ~(P3 * P2 * P1 * P0 + Cn); Сn == C0; P = R + S
		res->OVR = res->C4;
		break;
	case 4:
		res->P_ = false;
		res->G_ = !(G3 || G2 || G1 || G0);
		// G3 + G2 + G1 + G0 + Cn; Сn == C0; G = R * S
		res->C4 = G3 || G2 || G1 || G0 || c0;

		// res->C4 = ((ops->R & 0b1000 & ops->S & 0b1000) | (ops->R & 0b0100 & ops->S & 0b0100) |
		// 	(ops->R & 0b0010 & ops->S & 0b0010) | (ops->R & 0b0001 & ops->S & 0b0001)) | c0;

		// G3 + G2 + G1 + G0 + Cn; Сn == C0; G = R * S
		res->OVR = res->C4;
		break;
	case 5:
		res->P_ = false;
		res->G_ = !(notR_G3 || notR_G2 || notR_G1 || notR_G0);
		// Так же, как в 4, но все R заменены на ~R
		res->C4 = notR_G3 || notR_G2 || notR_G1 || notR_G0 || c0;
		res->OVR = res->C4;
		// res->C4 = ((~ops->R & 0b1000 & ops->S & 0b1000) | (~ops->R & 0b0100 & ops->S & 0b0100) |
		// 	(~ops->R & 0b0010 & ops->S & 0b0010) | (~ops->R & 0b0001 & ops->S & 0b0001)) | c0;
		//
		// res->OVR = (~(ops->R & 0b1000 & ops->S & 0b1000) | (~ops->R & 0b0100 & ops->S & 0b0100) |
		// 	(~ops->R & 0b0010 & ops->S & 0b0010) | (~ops->R & 0b0001 & ops->S & 0b0001)) | c0;
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

		// res->C4 = ~((~ops->R & 0b1000 & ops->S & 0b1000) | ((~ops->R & 0b1000 | ops->S & 0b1000) & (~ops->R & 0b0100 & ops->S & 0b0100)) |
		// 	((~ops->R & 0b1000 | ops->S & 0b1000) & (~ops->R & 0b0100 | ops->S & 0b0100) & (~ops->R & 0b0010 & ops->S & 0b0010)) |
		// 	(((~ops->R & 0b1000 | ops->S & 0b1000) & (~ops->R & 0b0100 | ops->S & 0b0100) & (~ops->R & 0b0010 | ops->S & 0b0010) & (~ops->R & 0b0001 | ops->S & 0b0001)) &
		// 		((~ops->R & 0b0001 & ops->S & 0b0001) | ~c0)));
		//
		// res->OVR = (~(~ops->R & 0b0100 | ops->S & 0b0100) |
		// 	~(~ops->R & 0b0100 & ops->S & 0b0100) & ~(~ops->R & 0b0010 & ops->S & 0b0010) & ~(~ops->R & 0b0001 | ops->S & 0b0001) |
		// 	~(~ops->R & 0b0100 & ops->S & 0b0100) & ~(~ops->R & 0b0010 & ops->S & 0b0010) & ~(~ops->R & 0b0001 & ops->S & 0b0001) & c0)
		// 	^
		// 	(~(~ops->R & 0b1000 | ops->S & 0b1000) |
		// 		~(~ops->R & 0b1000 & ops->S & 0b1000) & ~(~ops->R & 0b0100 | ops->S & 0b0100) |
		// 		~(~ops->R & 0b1000 & ops->S & 0b1000) & ~(~ops->R & 0b0100 & ops->S & 0b0100) & ~(~ops->R & 0b0010 | ops->S & 0b0010) |
		// 		~(~ops->R & 0b1000 & ops->S & 0b1000) & ~(~ops->R & 0b0100 & ops->S & 0b0100) & ~(~ops->R & 0b0010 & ops->S & 0b0010) & ~(~ops->R & 0b0001 | ops->S & 0b0001) |
		// 		~(~ops->R & 0b1000 & ops->S & 0b1000) & ~(~ops->R & 0b0100 & ops->S & 0b0100) & ~(~ops->R & 0b0010 & ops->S & 0b0010) & ~(~ops->R & 0b0001 & ops->S & 0b0001) & c0);
		break;
	case 7:
		res->P_ = G3 || G2 || G1 || G0;
		res->G_ = G3 || P3 && G2 || P3 && P2 && G1 || P3 && P2 && P1 && P0;
		// ~(G3 + P3 * G2 + P3 * P2 * G1 + P3 * P2 * P1 * P0 * (G1 + ~Cn)); Сn == C0
		res->C4 = !(G3 || P3 && G2 || P3 && P2 && G1 || P3 && P2 && P1 && P0 && (G1 || !c0));

		// res->C4 = ~((ops->R & 0b1000 & ops->S & 0b1000) | ((ops->R & 0b1000 | ops->S & 0b1000) & (ops->R & 0b0100 & ops->S & 0b0100)) |
		// 	((ops->R & 0b1000 | ops->S & 0b1000) & (ops->R & 0b0100 | ops->S & 0b0100) & (ops->R & 0b0010 & ops->S & 0b0010)) |
		// 	(((ops->R & 0b1000 | ops->S & 0b1000) & (ops->R & 0b0100 | ops->S & 0b0100) & (ops->R & 0b0010 | ops->S & 0b0010) & (ops->R & 0b0001 | ops->S & 0b0001)) &
		// 		((ops->R & 0b0001 & ops->S & 0b0001) | ~c0)));

		// (~P2 + ~G2 * ~P1 + ~G2 * ~G1 * ~P0 + ~G2 * ~G1 * ~G0 * Cn) ^ 
		// (~P3 + ~G3 * ~P2 + ~G3 * ~G2 * ~P1 + ~G3 * ~G2 * ~G1 * ~P0 + ~G3 * ~G2 * ~G1 * ~G0 * Cn)
		res->OVR = (!P2 || !G2 && !P1 || !G2 && !G1 && !P0 || !G2 && !G1 && !G0 && c0) !=
			(!P3 || !G3 && !P2 || !G3 && !G2 && !P1 || !G3 && !G2 && !G1 && !P0 || !G3 && !G2 && !G1 && !G0 && c0);
		// res->OVR = (~(ops->R & 0b0100 | ops->S & 0b0100) |
		// 	~(ops->R & 0b0100 & ops->S & 0b0100) & ~(ops->R & 0b0010 & ops->S & 0b0010) & ~(ops->R & 0b0001 | ops->S & 0b0001) |
		// 	~(ops->R & 0b0100 & ops->S & 0b0100) & ~(ops->R & 0b0010 & ops->S & 0b0010) & ~(ops->R & 0b0001 & ops->S & 0b0001) & c0)
		// 	^
		// 	(~(ops->R & 0b1000 | ops->S & 0b1000) |
		// 		~(ops->R & 0b1000 & ops->S & 0b1000) & ~(ops->R & 0b0100 | ops->S & 0b0100) |
		// 		~(ops->R & 0b1000 & ops->S & 0b1000) & ~(ops->R & 0b0100 & ops->S & 0b0100) & ~(ops->R & 0b0010 | ops->S & 0b0010) |
		// 		~(ops->R & 0b1000 & ops->S & 0b1000) & ~(ops->R & 0b0100 & ops->S & 0b0100) & ~(ops->R & 0b0010 & ops->S & 0b0010) & ~(ops->R & 0b0001 | ops->S & 0b0001) |
		// 		~(ops->R & 0b1000 & ops->S & 0b1000) & ~(ops->R & 0b0100 & ops->S & 0b0100) & ~(ops->R & 0b0010 & ops->S & 0b0010) & ~(ops->R & 0b0001 & ops->S & 0b0001) & c0);
		break;
	}
}