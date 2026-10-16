#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

// 10
// 5
// -1
// 20
// -2
// 7
// 0

// 10
// -5
// 0

// Интерактор отправляет по одному целому числу:
// - положительное число `L` - добавить трубу длины `L`;
// - отрицательное число `-K` - удалить трубу с `PID = K`;
// - `0` - закончить работу.
enum CommandType {
    ADD_PIPE = 1,
    REMOVE_PIPE = 2,
    END = 0
};

class Randomizer {
private:
    int MIN_PIPE_LENGTH = 1;
    int MAX_PIPE_LENGTH = 40;

    std::mt19937 gen{std::random_device{}()};
    int nextPid = 1;
    std::vector<int> activePids;

    int RandomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }


public:
    int generatePipe() {
        return RandomInt(MIN_PIPE_LENGTH, MAX_PIPE_LENGTH);
    }

    int generateRemovalPipe() {
        if (activePids.empty()) {
            return -1; 
        }
        int lastIndex = static_cast<int>(activePids.size()) - 1;
        int randomIndex = RandomInt(0, lastIndex);
        return activePids[randomIndex];
    }
    int chooseCommand() {
        if (activePids.empty()) {
            return ADD_PIPE; 
        }
        return RandomInt(0, 1) == 0 ? ADD_PIPE : REMOVE_PIPE;
    }

    std::vector<int> generateTest(int n) {
        std::vector<int> commands;

        for (int i = 0; i < n; ++i) {
            int cmd = chooseCommand();

            if (cmd == ADD_PIPE) {
                int pipeLength = generatePipe();
                commands.push_back(pipeLength);
                activePids.push_back(nextPid++);
            } else if (cmd == REMOVE_PIPE) {
                int pid = generateRemovalPipe();

                if (pid != -1) {
                    commands.push_back(-pid);

                    auto it = std::find(activePids.begin(), activePids.end(), pid);
                    if (it != activePids.end()) {
                        activePids.erase(it);
                    }
                }
            }
        }

        commands.push_back(END);
        return commands;
    }
};

int main() {
    Randomizer randomizer;

    int commandCount = 20;
    std::vector<int> commands = randomizer.generateTest(commandCount);

    for (int command : commands) {
        std::cout << command << '\n';
    }

    return 0;
}
