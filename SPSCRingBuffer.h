#pragma once
#include "basics.h"
#include <array>
#include <optional>
#include <atomic>
#include "MarketDataGenerator.h"
#include <new>

struct alignas(64) AlignedIndex
{
    std::atomic<std::size_t> value{0};
};

class SPSCRingBuffer
{
private:
    static constexpr std::size_t Capacity = 1024;
    
    // fixed-size array containing Capacity number of elements
    // array<int, 2> means we create: [int] [int]
    std::array<MarketEvent, Capacity> buffer_;
    // buffer does not need ot be atomic - only producer writes into a slot, consumer reads
    AlignedIndex readIndex_{0}; // safe concurent access because of atomic
    AlignedIndex writeIndex_{0};
    


public:
    bool push(const MarketEvent& event);
    std::optional<MarketEvent> pop();
    bool empty() const;
    bool full() const;
    
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

 
 false sharing
 producer modifise writeIndex, consumer modifies readIndex
 problem: they are probably physically adjacent in memory.
 | readIndex_ | writeIndex_ | ...
 CPU cores operate in cache lines.. of both atomics occupy the same cache line,
 then they can interfere with one another because they are modifying differeny bytes
 of the SAME cache line. this is false sharing. 'false' because from cpp programmers
 perspectivy, threads arent sharing the same variable
 
 CPU try to keep frequently accessed memory close to the executing core. if the following
 writeIndex_.load();
 writeIndex_.store(...);
 is executed a lot, then the relevant memory is in a cache close to that CPU core.
 Common cache line is 64 bytes. size_t is probably 8 bytes. thus both atomics can occupy the same line
 
 Say producer  is done by core A (writes writeIndex), consumer is done by core B (readIndex)
 but physically both cores can modify the same cache line.
 For a core to modify a particular line of cache, it needs to own it. hence the cache line ping-pongs between cores
 this is false sharing as if the cores were sharing the state (but this is not the case)
 in the case of SPSC buffer, the same cache line keeps changing upon writing to it and reading it
 
 However, we cannot completely eliminate communication because the producer reads readIndex, and consumer
 reads writeIndex.
 
 
 */
