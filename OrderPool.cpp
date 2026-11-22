#include "OrderPool.h"

OrderPool::OrderPool()
{
    freeSlots_.reserve(Capacity);
    
    for(size_t i = 0; i < Capacity; i++)
    {
        freeSlots_.push_back(i);
        isFree_[i] = true;
    }
}

std::optional<std::size_t> OrderPool::acquireSlot()
{
    if (freeSlots_.empty())
    {
        return std::nullopt;
    }

    std::size_t slot = freeSlots_.back();
    isFree_[slot] = false;
    freeSlots_.pop_back();

    return slot;
}

void OrderPool::releaseSlot(std::size_t slot)
{
    // prevents something like slot=999 for a 5-slot pool
    if(slot >= Capacity)
        return;
    
    if(isFree_[slot] == true)
        return;
    
    isFree_[slot] = true;
    freeSlots_.push_back(slot);
    

}
