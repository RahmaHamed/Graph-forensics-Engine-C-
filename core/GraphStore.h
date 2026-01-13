#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <atomic>
#include <string>
#include <map>
#include "core/Node.h"
#include "core/Edge.h"
#include "concurrency/RWLock.h"

namespace graph {
class GraphStore {
public:
    using NodeId = uint64_t;

    NodeId addNode(std::string label) {
        auto lock = lock_.write();
        auto id = nodeCounter_++;
        nodes_[id] = std::make_unique<Node>(id, label);
        return id;
    }

    void addEdge(NodeId source, NodeId target, long long timestamp) {
        auto lock = lock_.write();
        auto id = edgeCounter_++;
        auto edge = std::make_shared<Edge>(id, source, target, timestamp);
        edges_[id] = edge;
        adjacency_[source].push_back(edge);
        // Also add to target for "undirected" importance
        inbound_count_[target]++; 
    }

    // NEW: Find the most connected node in a time range
    std::pair<NodeId, int> getMostImportant(long long start, long long end) {
        auto lock = lock_.read();
        std::unordered_map<NodeId, int> scores;
        NodeId topNode = 0;
        int maxScore = -1;

        for (auto const& [src, edgeList] : adjacency_) {
            for (auto const& edge : edgeList) {
                if (edge->timestamp() >= start && edge->timestamp() <= end) {
                    scores[edge->source()]++;
                    scores[edge->target()]++;
                }
            }
        }

        for (auto const& [id, score] : scores) {
            if (score > maxScore) {
                maxScore = score;
                topNode = id;
            }
        }
        return {topNode, maxScore};
    }

    std::string getNodeLabel(NodeId id) {
        auto lock = lock_.read();
        if (nodes_.count(id)) return nodes_[id]->label();
        return "Unknown";
    }

    std::map<long long, std::vector<std::shared_ptr<Edge>>> getChronologicalEdges() {
        auto lock = lock_.read();
        std::map<long long, std::vector<std::shared_ptr<Edge>>> timeline;
        for (auto const& [id, edge] : edges_) {
            timeline[edge->timestamp()].push_back(edge);
        }
        return timeline;
    }

private:
    std::unordered_map<NodeId, std::unique_ptr<Node>> nodes_;
    std::unordered_map<uint64_t, std::shared_ptr<Edge>> edges_;
    std::unordered_map<NodeId, std::vector<std::shared_ptr<Edge>>> adjacency_;
    std::unordered_map<NodeId, int> inbound_count_;
    std::atomic<NodeId> nodeCounter_{0};
    std::atomic<uint64_t> edgeCounter_{0};
    RWLock lock_;
};
}
