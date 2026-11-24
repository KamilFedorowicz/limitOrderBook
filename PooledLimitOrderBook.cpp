#include "PooledLimitOrderBook.h"

void PooledLimitOrderBook::add(const Order& order)
{
    if(order.side == Side::Buy)
    {
        if(orderIndex_.find(order.id) != orderIndex_.end())
        {
            // the ID already exists
            return;
        }
        
        auto slot = orderPool_.add(order);
        
        if(!slot.has_value())
        {
            return;
        }
        
        // try_emplace will create a new price level if that does not already exist
        auto [levelIter, inserted] = bids_.try_emplace(
            order.price,   // KEY for the map
            order.price    // argument passed to PriceLevel constructor
        );

        PooledPriceLevel& level = levelIter->second; // reference to the actual price level in the map
        level.addSlot(slot.value());
        PooledOrderLocation location{order.side, order.price, slot.value()};
        orderIndex_.insert({order.id, location});
        
    
    }
    else
    {
        
    }
}
