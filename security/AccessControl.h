#pragma once
#include <string>

namespace graph {
    class AccessControl {
    public:
        static bool canWrite(const std::string& token) {
            return token == "ADMIN_SECRET_123"; // Simple gatekeeper
        }
    };
}
