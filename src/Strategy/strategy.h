#ifndef TECHARENA2026_STRATEGY_H
#define TECHARENA2026_STRATEGY_H

#include "instruction.h"

class Strategy {
public:
    Strategy() = default;
    ~Strategy() = default;

    Result* process(Instruction& instr);

    void reset() {}
};

#endif // TECHARENA2026_STRATEGY_H