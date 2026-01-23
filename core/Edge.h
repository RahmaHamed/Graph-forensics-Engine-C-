#ifndef EDGE_H
#define EDGE_H
#include <cstdint>

namespace graph {
class Edge {
public:
    Edge(uint64_t src, uint64_t tgt, long long ts) 
        : source_(src), target_(tgt), timestamp_(ts) {}
    uint64_t source() const { return source_; }
    uint64_t target() const { return target_; }
    long long timestamp() const { return timestamp_; }
private:
    uint64_t source_;
    uint64_t target_;
    long long timestamp_;
};
}
#endif
