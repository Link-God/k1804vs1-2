#include "Header.h"

int main() {
    auto res = new ALUReasult();
    auto ops = new Operands();
    ops->R = 0b0000; // ����������� ��� ��� ����
    ops->S = 0b0000; // ����������� ��� ��� ����
    computeFlags(res, false, ops, 0); // ��� ����� �������
    res->Z == 0; // ��� �������� ������
    // ...

    delete res;
    delete ops;
}