#pragma once
#include <map>
#include <functional>
#include <optional>
#include "basics.h"

class LimitOrderBook
{
public:
    void add(const Order& order);
    void printBids() const;
    void printAsks() const;
    std::optional<Price> bestBid() const;
    std::optional<Price> bestAsk() const;
    
private:
    // map sorts ascending, so thats good because lowest ask (sell) is best
    // hence asks_.begin() is the best ask
    std::map<Price, PriceLevel> asks_;
    std::map<Price, PriceLevel, std::greater<Price>> bids_;
    
    // now both bids.begin() and asks.begin() give highest bid and lowest ask, respectively
    
};
