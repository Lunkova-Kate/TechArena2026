#include <map>
#include <iostream>
#include <iterator>
#include <string>

using namespace std;

enum TypeInstruction {
    PutPipe,
    RemovePipe,
    End
};

enum TypeResult{
    OkPut,
    OkRemove
};

class Instruction;

class Result{
private:
    TypeResult _type;
    int _length;
    int _distance;
public:
    TypeResult getType();
    Result(TypeResult type, int len, int dist);
    ~Result();
    int getLength();
    int getDistance();
    void setLength(int len);
    void setDistance(int dist);
};

class Instruction{
private:
    TypeInstruction _type;
    int _x;
    Result *_res;
public:
    TypeInstruction getType();
    int getX();
    Instruction(string instruction);
    ~Instruction();
    Result* getResult();
    void setResult(Result *res);
};

Instruction::Instruction(string instruction){
    _res = nullptr;
    if(instruction.find("-", 0) == instruction.npos){
        _x = stoi(instruction);
        if(_x == 0){
            _type = End;
        }
        else{
            _type = PutPipe;
            _res = new Result(OkPut, 0, 0);
        }
    }
    else{
        _x = stoi(instruction.substr(1));
        _type = RemovePipe;
        _res = new Result(OkRemove, 0, 0);
    }
}

TypeInstruction Instruction::getType() {
    return Instruction::_type;
}

Instruction::~Instruction(){
    delete _res;
}

int Instruction::getX() {
    return _x;
}

void Instruction::setResult(Result *res) {
    delete _res;
    _res = res;
}

Result* Instruction::getResult() {
    return _res;
}

TypeResult Result::getType() {
    return _type;
}

Result::Result(TypeResult type, int len, int dist){
    _type = type;
    _length = len;
    _distance = dist;
}

Result::~Result(){}

int Result::getLength(){
    return _length;
}

int Result::getDistance(){
    return _distance;
}

void Result::setLength(int len) {
    _length = len;
}

void Result::setDistance(int dist) {
    _distance = dist;
}

typedef struct Pipe {
    int pid;
    int length;
    int offset;
} Pipe_t;


class Strategy {
    int _lengthTrench;
    map<int, Pipe_t> _pipesInfo; 
    map<int, int> _freeSegments; 
    int _nextPid = 1;
    int _lastPutLength = -1;
    int _sameLengthRun = 0;

    Result* processPut(Instruction& instr);
    Result* processRemove(Instruction& instr);
public:
    Strategy();
    ~Strategy();

    Result* process(Instruction& instr);
};

Strategy::Strategy(): _lengthTrench(0), _pipesInfo(), _freeSegments() {}
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
    if (_lastPutLength == x) {
        ++_sameLengthRun;
    } else {
        _lastPutLength = x;
        _sameLengthRun = 1;
    }
    const bool placeRight = (_sameLengthRun % 2 == 0);

    int n = 0;
    if (_lengthTrench == 0) {
        while (_lengthTrench < x) {
            _lengthTrench += 20;
            n++;
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
        const int pipeOffset = placeRight
            ? startOffset + length - x
            : startOffset;

        int pid = _nextPid++;
        Pipe_t pipe{pid, x, pipeOffset};
        _pipesInfo[pid] = pipe;

        _freeSegments.erase(best);
        if (length > x) {
            if (placeRight) {
                _freeSegments[startOffset] = length - x;
            } else {
                _freeSegments[startOffset + x] = length - x;
            }
        }

        instr.getResult()->setLength(0);
        instr.getResult()->setDistance(pipeOffset);

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
        freeLen += 20;
    } while (freeLen < x);

    const int pipeOffset = placeRight
        ? offset + freeLen - x
        : offset;

    int pid = _nextPid++;
    Pipe_t pipe{pid, x, pipeOffset};
    _pipesInfo[pid] = pipe;

    if (tailFree) {
        _freeSegments.erase(prev(_freeSegments.end()));
    }
    if (freeLen > x) {
        if (placeRight) {
            _freeSegments[offset] = freeLen - x;
        } else {
            _freeSegments[offset + x] = freeLen - x;
        }
    }

    instr.getResult()->setLength(n);
    instr.getResult()->setDistance(pipeOffset);

    return instr.getResult();
}

Result* Strategy::processRemove(Instruction& instr) {
    _lastPutLength = -1;
    _sameLengthRun = 0;

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

    return instr.getResult();
}



int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Strategy strategy;

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;

        Instruction instr(line);

        if (instr.getType() == End) {
            break;
        }

        Result* res = strategy.process(instr);

        if (res == nullptr) {
            break;
        }

        if (instr.getType() == PutPipe) {
            cout << res->getLength() << ' ' << res->getDistance() << '\n';
        } else if (instr.getType() == RemovePipe) {
            cout << res->getDistance() << '\n';
        }

        cout.flush();
    }

    return 0;
}
