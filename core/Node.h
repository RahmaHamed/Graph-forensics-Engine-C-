#ifndef NODE_H
#define NODE_H
#include <string>
#include <map>
#include <cstdint>

namespace graph {
class Node {
public:
    Node(uint64_t id, std::string label) : id_(id), label_(label), image_path_("default.png") {}
    uint64_t id() const { return id_; }
    std::string label() const { return label_; }
    void setImage(std::string path) { image_path_ = path; }
    std::string image() const { return image_path_; }
    std::map<std::string, std::string> properties;
private:
    uint64_t id_;
    std::string label_;
    std::string image_path_;
};
}
#endif
