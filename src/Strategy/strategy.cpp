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

        instr.getResult()->setLength(n);
        instr.getResult()->setDistance(0);

        return instr.getResult();
    }

    findZeroSegment();
    int maxLength = maxLengthFreeSegment();

    for (const auto& segment : _freeSegments) {
        int startOffset = segment.first;
        int length = segment.second;

        // если длина сегмента равна длине трубы
        if (length == instr.getX()) {
            for (int i = startOffset; i < startOffset + instr.getX(); ++i) {
                _pipes.set(i);
            }

            int pid = _nextPid++;
            Pipe_t pipe{pid, instr.getX(), startOffset};
            _pipesInfo[pid] = pipe;

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

            instr.getResult()->setLength(0);
            instr.getResult()->setDistance(startOffset);

            return instr.getResult();
        }
    }

    // если не найдено подходящего сегмента, расширяем траншею
    if (maxLength < instr.getX()) {
        int oldLength = _lengthTrench;

        do {
            _lengthTrench += 20;
            ++n;
            _pipes.resize(_lengthTrench);

            findZeroSegment();
            maxLength = maxLengthFreeSegment();
        } while (maxLength < instr.getX());

        int startOffset = oldLength;

        for (int i = startOffset; i < startOffset + instr.getX(); ++i) {
            _pipes.set(i);
        }

        int pid = _nextPid++;
        Pipe_t pipe{pid, instr.getX(), startOffset};
        _pipesInfo[pid] = pipe;

        instr.getResult()->setLength(n);
        instr.getResult()->setDistance(startOffset);

        return instr.getResult();
    }

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

    Result* result = new Result(OkRemove, pipe.length, pipe.offset);
    return result;
}

void Strategy::findZeroSegment() {
    _freeSegments.clear();

    int currentLength = 0;
    int startOffset = -1;

    for (int i = 0; i < _lengthTrench; ++i) {
        if (!_pipes[i]) {
            if (currentLength == 0) {
                startOffset = i;
            }
            ++currentLength;
        } else {
            if (currentLength > 0) {
                _freeSegments[startOffset] = currentLength;
            }

            currentLength = 0;
            startOffset = -1;
        }
    }

    if (currentLength > 0) {
        _freeSegments[startOffset] = currentLength;
    }
}

int Strategy::maxLengthFreeSegment() {
    int maxLength = 0;
    for (const auto& segment : _freeSegments) {
        if (segment.second > maxLength) {
            maxLength = segment.second;
        }
    }
    return maxLength;
}

void Strategy::reset() {
    _pipes.clear();
}
