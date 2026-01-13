#pragma once
#include "core/Node.h"

namespace graph {
class Edge {
public:
    using EdgeId = uint64_t;

    Edge(EdgeId id, Node::NodeId src, Node::NodeId tgt, long long ts)
        : id_(id), source_(src), target_(tgt), timestamp_(ts) {}

    Node::NodeId source() const { return source_; }
    Node::NodeId target() const { return target_; }
    long long timestamp() const { return timestamp_; }

private:
    EdgeId id_;
    Node::NodeId source_;
    Node::NodeId target_;
    long long timestamp_;
};
}
