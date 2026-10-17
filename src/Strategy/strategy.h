#ifndef TECHARENA2026_STRATEGY_H
#define TECHARENA2026_STRATEGY_H

#include <map>
#include "instruction.h"

using namespace std;

typedef struct Pipe {
    int pid;
    int length;
    int offset;
} Pipe_t;


class Strategy {
    map<int, Pipe_t> _pipes;

    Result* processPut(Instruction& instr);
    Result* processRemove(Instruction& instr);
public:
    Strategy();
    ~Strategy();

    Result* process(Instruction& instr);

    void reset();
};

#endif // TECHARENA2026_STRATEGY_H