#pragma once

#include <map>
#include "PooledPriceLevel.h"


class PooledLimitOrderBook
{
public:
    void add(const Order& order);
    void modify(OrderId id, Quantity quantity);
    void cancel(OrderId id);
    
    std::optional<Price> bestBid() const;
    std::optional<Price> bestAsk() const;
    std::optional<Price> spread() const;

    std::optional<Quantity> quantityAtPrice(Side side, Price price) const;
    void process(const MarketEvent& event);
    bool validate() const;
    
    void displayOrders();
    
private:
    OrderPool orderPool_;
    
    std::map<Price, PooledPriceLevel> asks_;
    std::map<Price, PooledPriceLevel, std::greater<Price>> bids_;
    std::unordered_map<OrderId, PooledOrderLocation> orderIndex_;
    
};
