#include <exception>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "Chat.h"

//      Client

namespace {
    Client parseClient(std::ifstream& in, const int id) {
        int period;
        std::string message;

        in >> period;
        in >> message;

        return Client(id, period, message);
    }
}

Client::Client(const unsigned id, const unsigned period,
               const std::string& message)
        : id_(id)
        , period_(period)
        , message_(message)
{}

//      Threads function
namespace {
    std::string timeNow(std::chrono::system_clock::time_point timePoint) {
        auto time = std::chrono::system_clock::to_time_t(timePoint);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%X");

        return ss.str();
    }
}

void clientWorkingThread(Chat* chat, const Client& client) {
    while(!chat->isExit_) {
        //creating message
        std::ostringstream os;
        os  << timeNow(std::chrono::system_clock::now())  << " "
            << client.id_ << " " << client.message_;

        {
            std::lock_guard<std::mutex> lock(chat->mt_);
            chat->buf_.push_back(os.str());
        }
        chat->cv_.notify_one();

        std::this_thread::sleep_for(client.period_);
    }
}

void writerThread(Chat* chat, std::ofstream* out) {
    while (!chat->isExit_) {
        std::unique_lock<std::mutex> lock(chat->mt_);
        while(chat->buf_.empty() && !chat->isExit_)
            chat->cv_.wait_for(lock, std::chrono::seconds(2));

        if (!chat->buf_.empty()) {
            chat->out << chat->buf_.front() << "\n";
            chat->buf_.pop_front();
        }

    }
}

void Chat::parseSettings(const std::string& configPath) {
    std::ifstream in(configPath);
    if (!in) {
        std::string what = std::string("Can't open the file ") + configPath;

        throw std::exception(what.c_str());
    }

    //Parse config
    std::getline(in, outFilePath_);

    size_t count;
    in >> count;

    clients_.resize(count);
    for(size_t i = 0; i < count; ++i)
        clients_[i] = parseClient(in, i + 1);
}

void Chat::start() {
    out = std::ofstream(outFilePath_);

    //one bucket for writer thread
    threads_.resize(clients_.size() + 1);

    for(size_t i = 0; i < threads_.size() - 1; ++i)
        threads_[i] = new std::thread(clientWorkingThread, this, clients_[i]);
    threads_.back() = new std::thread(writerThread, this, &out);
}

void Chat::stop() {
    isExit_ = true;
    for(size_t i = 0; i < threads_.size() - 1; ++i)
        threads_[i]->detach();
    threads_.back()->join();

    for(size_t i = 0; i < threads_.size() - 1; ++i)
        delete threads_[i];
    delete threads_.back();

    threads_.clear();
    threads_.shrink_to_fit();
}
