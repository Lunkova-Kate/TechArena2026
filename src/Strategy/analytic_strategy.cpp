#include "strategy.h"
#include "instruction.h"

#include <iostream>
#include <iterator>

Strategy::Strategy() : _lengthTrench(0), _pipesInfo(), _freeSegments() {}

Strategy::~Strategy() {}

Result *Strategy::process(Instruction &instr) {
    switch (instr.getType()) {
        case PutPipe:
            return processPut(instr);
        case RemovePipe:
            return processRemove(instr);
        case End:
            return nullptr;
    }

    return nullptr;
}

Result *Strategy::processPut(Instruction &instr) {
    if (_countChecked > 0) {
        correctWeights();
    }

    auto segments = getSuitableFreeSegments(instr.getX());
    if (segments->empty()) {
        int len = 0;
        int offset = _lengthTrench;
        for (auto it: _freeSegments) {
            if (it.first + it.second == _lengthTrench) {
                offset = it.first;
                len = it.second;
                break;
            }
        }
        int n = 0;
        while (len < instr.getX()) {
            len += 20;
            _lengthTrench += 20;
            n++;
        }

        Pipe_t pipe{.pid =  _nextPid++, .length =  instr.getX(), .offset =  offset};
        _pipesInfo[pipe.pid] = pipe;

        auto it = _freeSegments.find(offset);
        if (it != _freeSegments.end()) {
            _freeSegments.erase(it);
        }
        _freeSegments[offset + instr.getX()] = _lengthTrench - instr.getX() - offset;

        delete segments;

        instr.getResult()->setLength(n);
        instr.getResult()->setDistance(offset);

        return instr.getResult();
    }
    auto place = filterSuitableSegments(segments);

    int offset = get<2>(place) ? get<0>(place) + get<1>(place) - instr.getX() : get<0>(place);
    Pipe_t pipe{.pid =  _nextPid++, .length =  instr.getX(), .offset =  offset};
    _pipesInfo[pipe.pid] = pipe;

    auto it = _freeSegments.find(get<0>(place));
    if (it != _freeSegments.end()) {
        if (it->first == get<0>(place)) {
            if (get<2>(place)) {
                _freeSegments[it->first] = it->second - instr.getX();
            } else {
                _freeSegments.erase(it);
                _freeSegments[it->first + instr.getX()] = it->second - instr.getX();
            }
        }
    }
    delete segments;

    instr.getResult()->setLength(0);
    instr.getResult()->setDistance(offset);

    return instr.getResult();
}

Result *Strategy::processRemove(Instruction &instr) {
    int pid = instr.getX();

    auto it = _pipesInfo.find(pid);

    if (it == _pipesInfo.end()) {
        return nullptr;
    }


    Pipe_t pipe = it->second;
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

    _countChecked++;

    return instr.getResult();
}

void Strategy::correctWeights() {
    _countChecked = 0;
    for (auto &it: _pipesInfo) {
        it.second.countChecked++;
        if (it.second.countChecked == 1) {
            it.second.weight = 2.0;
        } else {
            it.second.weight = it.second.weight > 1.0 ? it.second.weight - 0.1 : 1.0;
        }
    }
}

map<int, int> *Strategy::getSuitableFreeSegments(int length) {
    auto segments = new map<int, int>;
    for (auto it: _freeSegments) {
        if (!segments->empty() and it.first + it.second == _lengthTrench) {
            break;
        }
        int len = it.second - it.first;
        if (len > length) {
            if (len > length + 10 || len < length + 3) {
                (*segments)[it.first] = it.second;
            }
        }
    }
    return segments;
}

tuple<int, int, bool> Strategy::filterSuitableSegments(map<int, int> *segments) {
    auto prevPipe = _pipesInfo.find(_nextPid - 1);
    if (prevPipe != _pipesInfo.end() and prevPipe->second.countChecked == 0) {
        int start = prevPipe->second.offset;
        int end = prevPipe->second.offset + prevPipe->second.length;

        auto seg = segments->find(end);
        if (seg != segments->end()) {
            return {end, (*segments)[end], false};
        }

        for (auto it: *segments) {
            if (it.first + it.second == start) {
                return {it.first, it.second, true};
            }
        }
    }

    auto segmentsReverse = new map<int, int>;

    for (auto it: (*segments)) {
        (*segmentsReverse)[it.first + it.second] = it.second;
    }

    double weight = 1.0;
    int startSeg = -1;
    int endSeg = -1;
    bool place = false;
    for (auto pipes: _pipesInfo) {
        int start = pipes.second.offset;
        int end = start + pipes.second.length;

        auto it = segmentsReverse->find(start);
        if (it != segmentsReverse->end()) {
            if (pipes.second.weight > weight) {
                weight = pipes.second.weight;
                endSeg = it->first;
                startSeg = endSeg - it->second;
                place = true;
                if (weight == 2.0) {
                    break;
                }
            }
        }

        it = segments->find(end);
        if (it != segments->end()) {
            if (pipes.second.weight > weight) {
                weight = pipes.second.weight;
                startSeg = it->first;
                endSeg = startSeg + it->second;
                place = false;
                if (weight == 2.0) {
                    break;
                }
            }
        }
    }
    delete segmentsReverse;

    if (startSeg == -1 or endSeg == -1) {
        return {segments->begin()->first, segments->begin()->first + segments->begin()->second, false};
    }
    return {startSeg, endSeg, place};
}