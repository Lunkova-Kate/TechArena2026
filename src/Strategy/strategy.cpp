#include "strategy.h"
#include "instruction.h"

#include <iostream>
#include <iterator>

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
    const int x = instr.getX();
    int n = 0;
    if (_pipes.empty()) {
        while (_lengthTrench < x) {
            _lengthTrench += 20;
            n++;
        }

        _pipes.resize(_lengthTrench);

        for (int i = 0; i < x; ++i) {
            _pipes.set(i);
        }

        int pid = _nextPid++;
        Pipe_t pipe{pid, x, 0};
        _pipesInfo[pid] = pipe;

        if (pipe.length < _lengthTrench) {
            _freeSegments[pipe.length] = _lengthTrench - pipe.length;
        }

        instr.getResult()->setLength(n);
        instr.getResult()->setDistance(0);

        return instr.getResult();
    }

    auto best = _freeSegments.end();
    for (auto it = _freeSegments.begin(); it != _freeSegments.end(); ++it) {
        if (it->second < x) continue;
        if (best == _freeSegments.end() || it->second < best->second) {
            best = it;
            if (it->second == x) break;  
        }
    }

    if (best != _freeSegments.end()) {
        const int startOffset = best->first;
        const int length = best->second;

        for (int i = startOffset; i < startOffset + x; ++i) {
            _pipes.set(i);
        }

        int pid = _nextPid++;
        Pipe_t pipe{pid, x, startOffset};
        _pipesInfo[pid] = pipe;

        _freeSegments.erase(best);
        if (length > x) {
            _freeSegments[startOffset + x] = length - x;
        }

        instr.getResult()->setLength(0);
        instr.getResult()->setDistance(startOffset);

        return instr.getResult();
    }

    int freeLen = 0;
    int offset = _lengthTrench;
    bool tailFree = false;
    if (!_freeSegments.empty()) {
        auto last = prev(_freeSegments.end());
        if (last->first + last->second == _lengthTrench) {
            tailFree = true;
            freeLen = last->second;
            offset = last->first;
        }
    }

    do {
        _lengthTrench += 20;
        ++n;
        _pipes.resize(_lengthTrench);
        freeLen += 20;
    } while (freeLen < x);

    for (int i = offset; i < offset + x; i++) {
        _pipes.set(i);
    }

    int pid = _nextPid++;
    Pipe_t pipe{pid, x, offset};
    _pipesInfo[pid] = pipe;

    if (tailFree) {
        _freeSegments.erase(prev(_freeSegments.end()));
    }
    if (offset + x < _lengthTrench) {
        _freeSegments[offset + x] = freeLen - x;
    }

    instr.getResult()->setLength(n);
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

    int start = pipe.offset;
    int len = pipe.length;

    auto next = _freeSegments.lower_bound(start);   
    if (next != _freeSegments.end() && start + len == next->first) {
        len += next->second;
        next = _freeSegments.erase(next);
    }
    if (next != _freeSegments.begin()) {
        auto before = prev(next);                  
        if (before->first + before->second == start) {
            start = before->first;
            len += before->second;
            _freeSegments.erase(before);
        }
    }
    _freeSegments[start] = len;

    return instr.getResult();
}