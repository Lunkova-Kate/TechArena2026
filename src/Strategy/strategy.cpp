#include "strategy.h"
#include "instruction.h"

#include <iostream>

Strategy::Strategy(): _pipes(), _lengthTrench(0) {}
Strategy::~Strategy(){}

Result* Strategy::process(Instruction& instr){
    switch(instr.getType()){
        case PutPipe:
            return processPut(instr);
        case RemovePipe:
            return processRemove(instr);
        case End: return nullptr;
    }
}

Result* Strategy::processPut(Instruction &instr) {
    if(_pipes.empty()){
        int n = 0;
        while(_lengthTrench < instr.getX()){
            _lengthTrench += 20;
            n++;
        }

        instr.getResult()->setLength(n);
        instr.getResult()->setDistance(0);
        return instr.getResult();
    }


}

Result* Strategy::processRemove(Instruction &instr) {
    auto it = _pipes.find(instr.getX());
    if (it != _pipes.end()) {
        _pipes.erase(it);
    }

    instr.getResult()->setDistance(it->second.offset);
    return instr.getResult();
}

void Strategy::reset() {
    _pipes.clear();
}
