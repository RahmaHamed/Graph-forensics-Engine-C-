#pragma once
#include "core/Edge.h"
#include <string>

namespace graph {
    class TimelineRules {
    public:
        static std::string classify(long long ts) {
            if (ts < 1000000000) return "HISTORICAL";
            if (ts < 2000000000) return "CURRENT";
            return "FUTURE_PREDICTION";
        }
    };
}
