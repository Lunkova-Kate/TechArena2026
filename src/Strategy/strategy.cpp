#include "strategy.h"
#include "instruction.h"


Strategy::Strategy(): _pipes() {}
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

}

Result* Strategy::processRemove(Instruction &instr) {

}

void Strategy::reset() {
    _pipes.clear();
}
