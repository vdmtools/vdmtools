#include "metaiv.h"

int main(int, char**)
{
VDMGetDefaultRecInfoMap().NewTag(10, 1);
VDMGetDefaultRecInfoMap().SetSymTag(10, L"X`a");
VDMGetDefaultRecInfoMap().NewTag(11, 1);
VDMGetDefaultRecInfoMap().SetSymTag(11, L"X`b");

Record r1(10,1);
Record r2(11,1);
Record r3(11,1);
r3.SetField(1, Int(100));
r2.SetField(1, r3);
r1.SetField(1, r2);
wcout << "r1=" << r1 << endl;
}

