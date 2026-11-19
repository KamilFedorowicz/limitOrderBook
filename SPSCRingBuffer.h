#pragma once
#include "basics.h"
#include <array>
#include <optional>

class SPSCRingBuffer
{
private:
    static constexpr std::size_t Capacity = 5;
    
    // fixed-size array containing Capacity number of elements
    // array<int, 2> means we create: [int] [int]
    std::array<MarketEvent, Capacity> buffer_;
    std::size_t readIndex_ = 0;
    std::size_t writeIndex_ = 0;
    
    bool empty() const;
    bool full() const;

public:
    bool push(const MarketEvent& event);
    std::optional<MarketEvent> pop();
    
};

/*
SPSC means single producer single consumer
 architecture: MarketDataGenerator -> ONE producer -> ringu buffer -> ONE consumer -> LimitOrderBook
 ring buffer - fixed-size array where, when you reach the end, you wrap back to the begining
 we maintain 2 positions: readIndex_ is where consumer reads next, writeIndex_ is where producer writes next
 initially both start at 0. Producer adds A, writeIndex_ advances from 0 to 1. Add B and C, it further advances to 3
 ring buffers are good for no latency. there is no erasing elements, shifting elements and allocating another nodes.
 Imagine that ringBuffer has the size 5. Lets say writeIndex_ = 4, and we write an event.
 Afterwards, the next write position is 0 (not 5 - the slot does not exist). For capacity 5 we have: 0 -> 1, 1 -> 2,
 2 -> 3, 3 -> 4, 4 -> 0.
 
 Initially readIndex_  = 0, writeIndex_ = 0. That means empty, but it could happen that enough events write such that
 writeIndex_ wraps all the way around. This might also mean full.
 
 Initial implementation, we sacrifice one slot. readIndex_ == writeIndex_ means empty and
 (writeIndex_ + 1) % Capacity == readIndex_ means full.
 
 For capacity 5, max stored events is 4.
 read = 0
 write = 4

           +-----+-----+-----+-----+-----+
           |  A  |  B  |  C  |  D  |     |
           +-----+-----+-----+-----+-----+
             ^                       ^
            read                   write
*/
