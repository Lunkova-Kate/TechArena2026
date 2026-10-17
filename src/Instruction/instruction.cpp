#include "instruction.h"

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