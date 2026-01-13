#pragma once
#include <shared_mutex>
#include <mutex>

namespace graph {
class RWLock {
public:
    RWLock() = default;
    RWLock(const RWLock&) = delete;
    RWLock& operator=(const RWLock&) = delete;

    auto read() const { return std::shared_lock(mutex_); }
    auto write() { return std::unique_lock(mutex_); }

private:
    mutable std::shared_mutex mutex_;
};
}
