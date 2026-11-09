#include "LimitOrderBook.h"
#include <optional>

void LimitOrderBook::add(const Order& order)
{
    if(order.side == Side::Buy)
    {
        if(bids_.find(order.price) != bids_.end())
        {
            // price already exists
            PriceLevel& priceLevel = bids_.at(order.price);
            priceLevel.add(order);
        }
        else
        {
            // price does not exist
            PriceLevel priceLevel(order.price);
            priceLevel.add(order);
            bids_.insert({order.price, priceLevel});
        }
    }
    else // order.side == Side::Sell
    {
        if(asks_.find(order.price) != asks_.end())
        {
            // price already exists
            PriceLevel& priceLevel = asks_.at(order.price);
            priceLevel.add(order);
        }
        else
        {
            // price does not exist
            PriceLevel priceLevel(order.price);
            priceLevel.add(order);
            asks_.insert({order.price, priceLevel});
        }
        
    }
}

void LimitOrderBook::printBids() const
{
    for(const auto& [price, priceLevel]: bids_)
    {
        std::cout << "Price level: " << price << '\n';
        priceLevel.printOrders();
    }
}

void LimitOrderBook::printAsks() const
{
    for(const auto& [price, priceLevel]: asks_)
    {
        std::cout << "Price level: " << price << '\n';
        priceLevel.printOrders();
    }
}

// this communicates: book has bids, optional has a price
// if it does not have bids, optional has nothing
std::optional<Price> LimitOrderBook::bestBid() const
{
    if(bids_.empty())
        return std::nullopt;
    
    return bids_.begin()->first;
}

std::optional<Price> LimitOrderBook::bestAsk() const
{
    if(asks_.empty())
        return std::nullopt;
    
    return asks_.begin()->first;
}
