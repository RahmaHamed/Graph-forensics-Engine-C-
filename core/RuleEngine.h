#pragma once
#include "core/Edge.h"
#include <string>

namespace graph {

enum class TimelineColor {
    GRAY,   // Default/Unknown
    BRONZE, // Ancient (< Year 1000)
    SILVER, // Medieval (1000 - 1900)
    GOLD,   // Modern (1900 - 2100)
    NEON    // Future (> 2100)
};

class RuleEngine {
public:
    static TimelineColor getColorForEdge(long long timestamp) {
        // Simple timeline logic based on Unix Epoch or Years
        if (timestamp < 31536000000LL) return TimelineColor::BRONZE; 
        if (timestamp < 63072000000LL) return TimelineColor::SILVER;
        if (timestamp < 1735689600LL)  return TimelineColor::GOLD;
        return TimelineColor::NEON;
    }

    static std::string colorToString(TimelineColor color) {
        switch(color) {
            case TimelineColor::BRONZE: return "BRONZE (Ancient)";
            case TimelineColor::SILVER: return "SILVER (Medieval)";
            case TimelineColor::GOLD:   return "GOLD (Modern)";
            case TimelineColor::NEON:   return "NEON (Future)";
            default: return "GRAY (Unknown)";
        }
    }
};

} // namespace graph
