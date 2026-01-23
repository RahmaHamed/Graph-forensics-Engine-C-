#ifndef GRAPH_STORE_H
#define GRAPH_STORE_H
#include "concurrency/RWLock.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "core/Node.h"
#include "core/Edge.h"
#include "concurrency/RWLock.h"
#include <unordered_set>
#include <algorithm>

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
    // --- Helper query methods used by CommandHandler / Investigation ---
    // Return a vector of neighbors (unique node IDs) for a given node.
    std::vector<uint64_t> getNeighbors(uint64_t id) {
        std::lock_guard<std::mutex> lock(edges_mutex_);
        std::unordered_set<uint64_t> s;
        for (auto const& [ts, list] : timeline_) {
            for (auto const& e : list) {
                if (e->source() == id) s.insert(e->target());
                else if (e->target() == id) s.insert(e->source());
            }
        }
        return std::vector<uint64_t>(s.begin(), s.end());
    }

    // Return common neighbors (intersection) of a and b.
    std::vector<uint64_t> getCommonNeighbors(uint64_t a, uint64_t b) {
        auto na = getNeighbors(a);
        auto nb = getNeighbors(b);
        std::unordered_set<uint64_t> setA(na.begin(), na.end());
        std::vector<uint64_t> common;
        for (auto x : nb) if (setA.count(x)) common.push_back(x);
        return common;
    }

    // Return all timestamps where there exists an edge between u and v (either direction).
    std::vector<long long> getAllTimestamps(uint64_t u, uint64_t v) {
        std::lock_guard<std::mutex> lock(edges_mutex_);
        std::vector<long long> ts;
        for (auto const& [t, list] : timeline_) {
            for (auto const& e : list) {
                if ((e->source() == u && e->target() == v) || (e->source() == v && e->target() == u)) {
                    ts.push_back(t);
                }
            }
        }
        std::sort(ts.begin(), ts.end());
        return ts;
    }

    // Return degree (number of connections across timeline) for a node.
    int getDegree(uint64_t id) {
        std::lock_guard<std::mutex> lock(edges_mutex_);
        int deg = 0;
        for (auto const& [t, list] : timeline_)
            for (auto const& e : list)
                if (e->source() == id || e->target() == id) ++deg;
        return deg;
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
