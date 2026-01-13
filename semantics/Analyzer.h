#pragma once
#include "core/Edge.h"
#include <string>

namespace graph {
class Analyzer {
public:
    static std::string getRiskProfile(long long timestamp) {
        // Example: Data older than a certain threshold is 'Legacy'
        if (timestamp < 1000000000) return "LEGACY_DATA";
        return "ACTIVE_REALTIME";
    }
};
}
