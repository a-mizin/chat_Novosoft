#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <fstream>

struct Client;

class Chat {
    friend void clientWorkingThread (Chat* chat, const  Client&   client);
    friend void writerThread        (Chat* chat, std::  ofstream* out);
public:
    Chat() = default;

    void parseSettings(const std::string& configPath);

    Chat(const Chat&    rhs) = delete;
    Chat(      Chat&&   rhs) = delete;

    void start();
    void stop();

private:
    //for multithreading
    bool                        isExit_ = false;
    std::mutex                  mt_;
    std::condition_variable     cv_;
    std::list<std::string>      buf_;
    std::vector<std::thread*>   threads_;

    //config info
    std::string                 outFilePath_;
    std::ofstream               out;
    std::vector<Client>         clients_;
};

struct Client {
    Client() = default;

    Client(const unsigned       id,
           const unsigned       period,
           const std::string&   message);

public:
                 unsigned   id_;
    std::chrono::seconds    period_;
    std::        string     message_;
};
