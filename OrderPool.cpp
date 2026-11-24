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

std::optional<std::size_t> OrderPool::add(const Order& order)
{
    std::optional<std::size_t> slot = acquireSlot();
    
    if(slot.has_value())
    {
        orders_[slot.value()] = order;
        return slot;
    }
    
    return std::nullopt;
}

Order* OrderPool::get(std::size_t slot)
{
    if(slot >= Capacity)
        return nullptr;
    
    if(isFree_[slot] == true)
        return nullptr;
    
    return &orders_[slot];
}

const Order* OrderPool::get(std::size_t slot) const
{
    if(slot >= Capacity)
        return nullptr;
    
    if(isFree_[slot] == true)
        return nullptr;
    
    return &orders_[slot];
}

/*
 // basically the release function
 bool OrderPool::remove(std::size_t slot)
 {
 if(slot >= Capacity)
 return false;
 
 if(isFree_[slot] == true)
 return false;
 
 isFree_[slot] = true;
 freeSlots_.push_back(slot);
 return true;
 }
 */
