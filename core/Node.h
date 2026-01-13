#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

namespace graph {
class Node {
public:
    using NodeId = uint64_t;
    using Metadata = std::unordered_map<std::string, std::string>;

    explicit Node(NodeId id, std::string label = "")
        : id_(id), label_(std::move(label)) {}

    NodeId id() const noexcept { return id_; }
    const std::string& label() const noexcept { return label_; }
private:
    const NodeId id_;
    std::string label_;
    Metadata metadata_;
};
}
