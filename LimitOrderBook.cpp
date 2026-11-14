#include "LimitOrderBook.h"
#include <optional>


void LimitOrderBook::add(const Order& order)
{
    if(order.side == Side::Buy)
    {
        if(orderIndex_.find(order.id) != orderIndex_.end())
        {
            // an order with this id already exists - reject it
            // need to check before modifying anything
            return;
        }
        /*
        The std::try_emplace() function, introduced in C++17, inserts a key-value pair into a std::map or std::unordered_map only if the key does not already exist.
         The function returns a std::pair<iterator, bool>.
         iterator: Points to the inserted element, or the existing element if the key already exists.
         bool: true if insertion succeeds; otherwise false.
         */
        auto [levelIter, inserted] = bids_.try_emplace(
            order.price,   // KEY for the map
            order.price    // argument passed to PriceLevel constructor
        );
        
        /*
         if the price level corresponding to the price (first argument) does not exist, a new PriceLevel was created and inserted = true.
         if the price level already exists, try_emplace does not create another PriceLevel, and inserted = falce
         the second order.price is used only if the key does not exist to call PriceLevel(order.price) such that we have some element corresponding to order.price in the bids map
         
         */
        
        PriceLevel& level = levelIter->second; // reference to the actual price level in the map
        auto orderIter = level.add(order);
        /*
         if inserted = true, then this function is adding the first element that corresponds to order.price. if inserted = false, then it is just simply adding another element
         */
        
        if(orderIter.has_value())
        {
            if(orderIndex_.find(order.id) != orderIndex_.end())
            {
                // an order with this id already exists - reject it
                return;
            }
            
            // orderIter.value() is an iterator pointing directly to the just added order
            OrderLocation ordLoc = OrderLocation(order.side, order.price, orderIter.value());
            orderIndex_.insert({order.id, ordLoc});
        }
        
    }
    else // order.side == Side::Sell
    {
        auto [levelIter, inserted] = asks_.try_emplace(
            order.price,   // KEY for the map
            order.price    // argument passed to PriceLevel constructor
        );
        
        
        PriceLevel& level = levelIter->second;
        auto orderIter = level.add(order);

        if(orderIter.has_value())
        {
            if(orderIndex_.find(order.id) != orderIndex_.end())
            {
                // an order with this id already exists - reject it
                return;
            }
            OrderLocation ordLoc = OrderLocation(order.side, order.price, orderIter.value());
            orderIndex_.insert({order.id, ordLoc});
        }
    }
}

void LimitOrderBook::modify(OrderId id, Quantity newQuantity)
{
    auto iter = orderIndex_.find(id);
    if(iter != orderIndex_.end() && newQuantity > 0)
    {
        Quantity& quant = iter->second.it->quantity;
        quant = newQuantity;
        /*
         more compact approach would be
         iter->second.it->quantity = newQuantity;
         */
    }
}

std::optional<Quantity> LimitOrderBook::quantityAtPrice(Side side, Price price) const
{
    if(side == Side::Buy)
    {
        auto it = bids_.find(price);
        if(it != bids_.end())
        {
            const PriceLevel& priceLevel = it->second;
            return priceLevel.total_quantity();
            /*
             more compact approach would be
             return it->second.total_quantity();
             */
            
        }
        return std::nullopt;
    }
    else // (side == Side::Buy)
    {
        auto it = asks_.find(price);
        if(it != asks_.end())
        {
            const PriceLevel& priceLevel = it->second;
            return priceLevel.total_quantity();
        
        }
        return std::nullopt;
    }

}
/*
the same order is stored in two data structures for different purposes.
 PriceLevel contains the roder because we care about orders at price, quantity etc
 orderIndex_ is just a fast lookup mechanism
 */
void LimitOrderBook::cancel(OrderId id)
{
    auto iter = orderIndex_.find(id);
    if(iter != orderIndex_.end())
    {
        
        // get OrderLocation
        Side side = iter->second.side;
        Price price = iter->second.price;
        auto orderIter = iter->second.it;
        
        if(side == Side::Buy)
        {
            PriceLevel& priceLevel = bids_.at(price);
            // remove the exact order using iterator
            priceLevel.remove(orderIter);
            
            // if the price level is empty, remove it from the list
            if(priceLevel.empty())
            {
                bids_.erase(price);
            }
        }
        else
        {
            // side == Side::Sell
            PriceLevel& priceLevel = asks_.at(price);
            priceLevel.remove(orderIter);
            
            if(priceLevel.empty())
            {
                asks_.erase(price);
            }
        }
        // remove ID from the order index
        orderIndex_.erase(id);
        
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

std::optional<Price> LimitOrderBook::spread() const
{
    auto bid = bestBid();
    auto ask = bestAsk();
    
    if(asks_.empty() || bids_.empty())
        return std::nullopt;
    
    return ask.value() - bid.value();
    
}
