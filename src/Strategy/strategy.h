#ifndef TECHARENA2026_STRATEGY_H
#define TECHARENA2026_STRATEGY_H

#include <map>
#include <set>
#include <vector>
#include "instruction.h"

using namespace std;

typedef struct Pipe {
    int pid;
    int length;
    int offset;
    int countChecked = 0;
    double weight = 1.0;
} Pipe_t;

class Strategy {
    int _lengthTrench;
    map<int, Pipe_t> _pipesInfo;
    map<int, int> _freeSegments;
    int _nextPid = 1;

    int _countChecked = 0;

    Result* processPut(Instruction& instr);
    Result* processRemove(Instruction& instr);
    void correctWeights();
    map<int, int>* getSuitableFreeSegments(int length);
    tuple<int, int, bool> filterSuitableSegments(map<int, int> *segments);
public:
    Strategy();
    ~Strategy();

    Result* process(Instruction& instr);
};

#endif // TECHARENA2026_STRATEGY_H