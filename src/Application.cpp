#include <iostream>
#include <string>

#include "instruction.h"
#include "strategy.h"


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Strategy strategy;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        if(line.find(' ') != std::string::npos){
            continue;
        }

        Instruction instr(line);

        if (instr.getType() == End) {
            break;
        }

        Result* res = strategy.process(instr);

        if (res == nullptr) {
            break;
        }

        if (instr.getType() == PutPipe) {
            std::cout << res->getLength() << ' ' << res->getDistance() << '\n';
        } else if (instr.getType() == RemovePipe) {
            std::cout << res->getDistance() << '\n';
        }

        std::cout.flush();
    }

    return 0;
}