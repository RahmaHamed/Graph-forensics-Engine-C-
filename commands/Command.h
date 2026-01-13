#pragma once
#include "core/GraphStore.h"

namespace graph {
class Command {
public:
    virtual ~Command() = default;
    virtual void execute(GraphStore& store) = 0;
};

class AddNodeCommand : public Command {
    std::string label_;
public:
    AddNodeCommand(std::string label) : label_(std::move(label)) {}
    void execute(GraphStore& store) override {
        store.addNode(label_);
    }
};
}
