// client.cpp - Runs on all other devices
// Compile: g++ -std=c++11 -Wno-psabi client.cpp -o client -lpthread
// Usage:   ./client <your_name> <hub_ip>
// Example: ./client Ali 192.168.1.37
#include "httplib.h"
#include <iostream>
#include <thread>
#include <chrono>

std::string name, hubIP;
size_t seen = 0; // number of messages printed so far

// Polls the hub once per second for new messages, in the background
void listenForNewMessages() {
    httplib::Client cli(hubIP, 8080);
    while (true) {
        httplib::Result res = cli.Get("/message?from=" + std::to_string(seen));
        if (res && !res->body.empty()) {
            std::cout << res->body << std::flush;
            for (char c : res->body)
                if (c == '\n') seen++; // one line = one message
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./client <your_name> <hub_ip>" << std::endl;
        return 1;
    }
    name = argv[1];
    hubIP = argv[2];

    std::thread listener(listenForNewMessages);

    httplib::Client cli(hubIP, 8080);
    std::string message;
    while (std::getline(std::cin, message)) {
        if (!cli.Post("/message", name + ": " + message, "text/plain"))
            std::cout << "Could not reach the hub!" << std::endl;
    }
    listener.join();
}
