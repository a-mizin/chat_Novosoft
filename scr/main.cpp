#include <chrono>
#include <iostream>
#include <thread>

#include "chatLib/Chat.h"

bool checkArgs(int argc, char** argv) {
    if (argc != 2) {
        if (argc >= 2)
            std::cerr << "Too many parameters.\n";
        if (argc == 1)
            std::cerr << "Path to file is not specified.\n";

        std::cerr << "Example of a program call: chat.exe [config-file]";

        return true;
    }

    return false;
}

int main(int argc, char** argv) {
    if (checkArgs(argc, argv))
        return -1;

    Chat chat;
    try {
        chat.parseSettings(std::string(argv[1]));
    }
    catch (std::exception& exception) {
        std::cout << exception.what();
    }

    chat.start();
    do {
        std::string command;
        std::cin >> command;

        if (command == "stop") {
            chat.stop();
            break;
        }
    } while (true);

    return 0;
}