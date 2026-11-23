#pragma once
#include <array>
#include "basics.h"


class OrderPool
{
public:
    OrderPool();
    std::optional<std::size_t> add(const Order& order);
    Order* get(std::size_t slot);
    void releaseSlot(std::size_t slot);

    
private:
    static constexpr std::size_t Capacity = 5;
    
    std::array<Order, Capacity> orders_;
    std::vector<std::size_t> freeSlots_;

    std::optional<std::size_t> acquireSlot();
    
    std::array<bool, Capacity> isFree_;
};

/*
 for a capacity of 5 we have something like:
 orders_

 index    0       1       2       3       4

        [Order] [Order] [Order] [Order] [Order]
 
 how do we know which slots are available?
 initially all slots ara available
 free slots: [0,1,2,3,4]. someone asks for a slot
 say, we take 4. now this is considered occupied.
 then slot is released with freeSlots_.push_back(4);
 we are back to [0,1,2,3,4]. acquiring a slot is equivalent
 to pop_back()
 */
