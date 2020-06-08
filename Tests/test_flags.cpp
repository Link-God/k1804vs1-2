#include "Header.h"

int main() {
    auto res = new ALUReasult();
    auto ops = new Operands();
    ops->R = 0b0000; // выставляете кой вам надо
    ops->S = 0b0000; // выставляете кой вам надо
    computeFlags(res, false, ops, 0); // тут можно дебагом
    res->Z == 0; // тут проверки флагов
    // ...

    delete res;
    delete ops;
}