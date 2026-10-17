#include "strategy.h"
#include "instruction.h"

#include <iostream>
#include <vector>

Strategy::Strategy(): _pipes(), _lengthTrench(0), _pipesInfo(), _freeSegments() {}
Strategy::~Strategy(){}

Result* Strategy::process(Instruction& instr){
    switch(instr.getType()){
        case PutPipe:
            return processPut(instr);
        case RemovePipe:
            return processRemove(instr);
        case End: return nullptr;
    }

    return nullptr;
}

Result* Strategy::processPut(Instruction &instr) {
    int n = 0;
    if (_pipes.empty()) {
        while (_lengthTrench < instr.getX()) {
            _lengthTrench += 20;
            n++;
        }

        _pipes.resize(_lengthTrench);

        for (int i = 0; i < instr.getX(); ++i) {
            _pipes.set(i);
        }

        int pid = _nextPid++;
        Pipe_t pipe{pid, instr.getX(), 0};
        _pipesInfo[pid] = pipe;

        if(pipe.length < _lengthTrench){
            _freeSegments[pipe.length] = _lengthTrench - pipe.length;
        }

        instr.getResult()->setLength(n);
        instr.getResult()->setDistance(0);

        return instr.getResult();
    }

    for (const auto& segment : _freeSegments) {
        int startOffset = segment.first;
        int length = segment.second;

        if (length == instr.getX()) {
            for (int i = startOffset; i < startOffset + instr.getX(); ++i) {
                _pipes.set(i);
            }

            int pid = _nextPid++;
            Pipe_t pipe{pid, instr.getX(), startOffset};
            _pipesInfo[pid] = pipe;

            _freeSegments.erase(_freeSegments.find(startOffset));

            instr.getResult()->setLength(0);
            instr.getResult()->setDistance(startOffset);

            return instr.getResult();
        }

        // Если свободный сегмент длиннее трубы
        if (length > instr.getX()) {
            for (int i = startOffset; i < startOffset + instr.getX(); ++i) {
                _pipes.set(i);
            }

            int pid = _nextPid++;
            Pipe_t pipe{pid, instr.getX(), startOffset};
            _pipesInfo[pid] = pipe;

            auto it = _freeSegments.find(startOffset);
            _freeSegments.erase(it);
            _freeSegments[startOffset + instr.getX()] = length - instr.getX();

            instr.getResult()->setLength(0);
            instr.getResult()->setDistance(startOffset);

            return instr.getResult();
        }
    }

    auto last = _freeSegments.rbegin();
    int freeLen = 0;
    int offset = _lengthTrench;
    if(last->first + last->second == _lengthTrench){
        freeLen = last->second;
        offset = last->first;
    }

    do {
        _lengthTrench += 20;
        ++n;
        _pipes.resize(_lengthTrench);
        freeLen += 20;

    } while (freeLen < instr.getX());

    for(int i = offset; i < offset + instr.getX(); i++){
        _pipes.set(i);
    }

    if(offset == last->first){
        _freeSegments.erase(prev(_freeSegments.end()));
    }
    if(offset + instr.getX() < _lengthTrench){
        _freeSegments[offset + instr.getX()] = freeLen - instr.getX();
    }

    instr.getResult()->setLength(0);
    instr.getResult()->setDistance(offset);

    return instr.getResult();
}

Result* Strategy::processRemove(Instruction& instr) {
    int pid = instr.getX();

    auto it = _pipesInfo.find(pid);

    if (it == _pipesInfo.end()) {
        return nullptr; 
    }

    Pipe_t pipe = it->second;

    for (int i = pipe.offset; i < pipe.offset + pipe.length; ++i) {
        _pipes.reset(i);
    }

    _pipesInfo.erase(it);

    instr.getResult()->setLength(pipe.length);
    instr.getResult()->setDistance(pipe.offset);

    auto newIt = _freeSegments.find(pipe.offset + pipe.length);
    if(pipe.offset > 0 && _pipes[pipe.offset - 1]){
        if(newIt != _freeSegments.end()){
            int len = newIt->second;
            _freeSegments.erase(newIt);
            _freeSegments[pipe.offset] = pipe.length + len;
        }
        else{
            _freeSegments[pipe.offset] = pipe.length;
        }
    }
    else{
        int pos = pipe.offset - 1;
        while(pos > 0 && _pipes[pos - 1]){
            pos--;
        }

        if(newIt != _freeSegments.end()){
            int len = newIt->second;
            _freeSegments.erase(newIt);
            _freeSegments[pos] = pipe.offset - pos + pipe.length + len;
        }
        else{
            _freeSegments[pos] = pipe.offset - pos + pipe.length;
        }
    }

    return instr.getResult();
}
