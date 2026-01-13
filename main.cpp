#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include "core/GraphStore.h"

void update_html(const std::vector<std::string>& nodes) {
    std::ofstream file("index.html");
    file << "<html><head><style>"
         << "body { background: #1a1a1a; color: white; font-family: sans-serif; text-align: center; }"
         << ".node { background: #007bff; padding: 10px; margin: 10px; border-radius: 5px; display: inline-block; }"
         << "</style></head><body>"
         << "<h1>��️ Graph Engine Static Dashboard</h1>"
         << "<h3>Current Nodes:</h3>";
    
    for (const auto& name : nodes) {
        file << "<div class='node'>" << name << "</div>";
    }

    file << "</body></html>";
    file.close();
    std::cout << "✨ index.html updated!" << std::endl;
}

int main() {
    graph::GraphStore store;
    std::vector<std::string> node_list;
    std::string pipeName = "graph_pipe";
    mkfifo(pipeName.c_str(), 0666);

    std::cout << "��️ Engine Online. Creating HTML on the fly..." << std::endl;
    update_html(node_list); // Create initial empty page

    while (true) {
        std::ifstream pipe(pipeName);
        std::string cmd;
        if (std::getline(pipe, cmd)) {
            if (cmd.substr(0, 3) == "add") {
                std::string label = cmd.substr(4);
                store.addNode(label);
                node_list.push_back(label);
                update_html(node_list); // Update the HTML file!
            }
        }
    }
    return 0;
}
