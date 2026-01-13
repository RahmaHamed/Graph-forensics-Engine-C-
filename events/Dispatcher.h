#pragma once
#include <iostream>
#include <string>
#include <thread>

namespace graph {
class Dispatcher {
public:
    static void onNodeAdded(uint64_t id, std::string label) {
        // Spawn a background thread for the event
        // std::jthread automatically joins (cleans up) when it goes out of scope
        std::jthread([id, label]() {
            // Simulate a slight delay (like a network log)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "\n[Async Event] �� Node [" << id << "] processed in background." << std::endl;
        }).detach(); 
    }
};
}
