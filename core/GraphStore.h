#ifndef GRAPH_STORE_H
#define GRAPH_STORE_H

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "core/Node.h"
#include "core/Edge.h"
#include "concurrency/RWLock.h"

namespace graph {

class GraphStore {
public:
    uint64_t addNode(std::string label) {
        std::lock_guard<std::mutex> lock(nodes_mutex_);
        uint64_t id = next_node_id_++;
        nodes_[id] = std::make_unique<Node>(id, label);
        return id;
    }

    void addEdge(uint64_t src, uint64_t tgt, long long ts) {
        std::lock_guard<std::mutex> lock(edges_mutex_);
        // FIXED: Removed the extra ID argument to match Edge.h
        auto edge = std::make_shared<Edge>(src, tgt, ts);
        timeline_[ts].push_back(edge);
    }

    std::unordered_map<uint64_t, std::unique_ptr<Node>>& getNodes() { return nodes_; }
    std::map<long long, std::vector<std::shared_ptr<Edge>>>& getChronologicalEdges() { return timeline_; }
    
    std::string getNodeLabel(uint64_t id) {
        if (nodes_.count(id)) return nodes_[id]->label();
        return "Unknown";
    }

private:
    uint64_t next_node_id_ = 0;
    std::unordered_map<uint64_t, std::unique_ptr<Node>> nodes_;
    std::map<long long, std::vector<std::shared_ptr<Edge>>> timeline_;
    std::mutex nodes_mutex_;
    std::mutex edges_mutex_;
};

}
#endif
