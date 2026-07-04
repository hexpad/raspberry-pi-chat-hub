// hub.cpp - Runs on the Raspberry Pi (central server)
// Compile: g++ -std=c++11 -Wno-psabi hub.cpp -o hub -lpthread
// Usage:   ./hub
#include "httplib.h"
#include <iostream>
#include <vector>
#include <mutex>

httplib::Server svr;
std::vector<std::string> messages;
std::mutex mtx;

// Called when a client sends a message; appends it to the list
void messageReceived(const httplib::Request& req, httplib::Response& res) {
    mtx.lock();
    messages.push_back(req.body);
    mtx.unlock();
    std::cout << req.body << std::endl;
}

// A client asks for all messages starting from index "from"
void serveMessages(const httplib::Request& req, httplib::Response& res) {
    size_t from = std::stoul(req.get_param_value("from"));
    std::string fresh;
    mtx.lock();
    for (size_t i = from; i < messages.size(); i++)
        fresh += messages[i] + "\n";
    mtx.unlock();
    res.set_content(fresh, "text/plain");
}

int main() {
    svr.Post("/message", messageReceived);
    svr.Get("/message", serveMessages);
    std::cout << "Hub listening on port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);
}
