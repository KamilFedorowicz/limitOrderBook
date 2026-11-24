#pragma once

#include <map>
#include "PooledPriceLevel.h"


class PooledLimitOrderBook
{
public:
    void add(const Order& order);
    
private:
    OrderPool orderPool_;
    
    std::map<Price, PooledPriceLevel> asks_;
    std::map<Price, PooledPriceLevel, std::greater<Price>> bids_;
    
    std::unordered_map<OrderId, PooledOrderLocation> orderIndex_;
    
};
