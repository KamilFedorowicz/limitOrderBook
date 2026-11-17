#pragma once
#include <deque>
#include "MarketDataGenerator.h"
#include <mutex>
#include <condition_variable>

class BoundedQueue
{
public:
    // explicit prevents accidental conversions
    explicit BoundedQueue(std::size_t capacity);
    void add(const MarketEvent& event);
    MarketEvent pop();
    std::size_t size();
    
    
private:
    std::size_t capacity;
    std::deque<MarketEvent> queue_;
    std::size_t capacity_;
    std::mutex mutex_;
    
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};
