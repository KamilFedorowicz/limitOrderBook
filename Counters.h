#pragma once

#include <atomic>

// requests that each PaddedAtomic starts on a 64-byte boundary
// effectively, instead of potentially using the same cache line, we are using different
struct alignas(64) PaddedAtomic
{
    std::atomic<std::uint64_t> value{0};
};

struct Counters
{
    PaddedAtomic a;
    PaddedAtomic b;
};
