#include "PooledPriceLevel.h"

PooledPriceLevel::PooledPriceLevel(Price price): price_(price){}

void PooledPriceLevel::addSlot(std::size_t slot)
{
    slots_.push_back(slot);
}

Quantity PooledPriceLevel::totalQuantity(const OrderPool& pool) const
{
    Quantity result = 0;
    
    // need to iterate only through the slots contained in this price level
    for(std::size_t slot : slots_)
    {
        const Order* order = pool.get(slot);
        
        if(order != nullptr)
        {
            result += order->quantity;
        }
    }
    
    return result;
    
}

bool PooledPriceLevel::empty() const
{
    return slots_.empty();
}

bool PooledPriceLevel::removeSlot(std::size_t slot)
{
    // removeSlot is currently O(n) because it is using find. ok for now
    auto it = std::find(slots_.begin(), slots_.end(), slot);
    
    if(it == slots_.end())
    {
        return false;
    }
    
    slots_.erase(it);
    return true;
}
