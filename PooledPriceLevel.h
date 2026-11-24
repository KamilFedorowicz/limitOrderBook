#pragma once
#include <algorithm>

#include "PooledOrderLocation.h"
#include "OrderPool.h"


class PooledPriceLevel
{
public:
    explicit PooledPriceLevel(Price price);
    void addSlot(std::size_t slot);
    Quantity totalQuantity(const OrderPool& pool) const;
    bool empty() const;
    bool removeSlot(std::size_t slot);
    
private:
    Price price_;
    std::list<std::size_t> slots_;
};
