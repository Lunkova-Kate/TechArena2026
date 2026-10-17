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
    int _lengthTrench;
    map<int, Pipe_t> _pipesInfo; // pid -> Pipe_t
    map<int, int> _freeSegments; // начало -> длина
    int _nextPid = 1;

    Result* processPut(Instruction& instr);
    Result* processRemove(Instruction& instr);
public:
    Strategy();
    ~Strategy();

    Result* process(Instruction& instr);
};

#endif // TECHARENA2026_STRATEGY_H
