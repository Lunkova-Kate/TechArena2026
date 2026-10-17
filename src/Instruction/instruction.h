#ifndef TECHARENA2026_INSTRUCTION_H
#define TECHARENA2026_INSTRUCTION_H

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

#endif //TECHARENA2026_INSTRUCTION_H