#include "PooledLimitOrderBook.h"

void PooledLimitOrderBook::add(const Order& order)
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
    
    if(order.side == Side::Buy)
    {

        
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
        // try_emplace will create a new price level if that does not already exist
        auto [levelIter, inserted] = asks_.try_emplace(
            order.price,   // KEY for the map
            order.price    // argument passed to PriceLevel constructor
        );

        PooledPriceLevel& level = levelIter->second; // reference to the actual price level in the map
        level.addSlot(slot.value());
        PooledOrderLocation location{order.side, order.price, slot.value()};
        orderIndex_.insert({order.id, location});
    }
}

void PooledLimitOrderBook::modify(OrderId id, Quantity quantity)
{
    // this is basically cancel operation
    if(quantity == 0)
    {
        return;
    }
    
    // returns std::unordered_map<OrderId, PooledOrderLocation>
    auto iter = orderIndex_.find(id);
    if(iter == orderIndex_.end())
    {
        return;
    }

    // there is an ID that can be modified
    std::size_t slot = iter->second.slot;
    Order* order = orderPool_.get(slot);
    
    // this should never happen because it
    // means that index and pool are inconsistent
    if(order == nullptr)
    {
        return;
    }
    
    order->quantity = quantity;
        
}

void PooledLimitOrderBook::cancel(OrderId id)
{
    auto iter = orderIndex_.find(id);
    if(iter == orderIndex_.end())
    {
        return;
    }
    
    auto orderLocation = iter->second;
    Price price = orderLocation.price;
    Side side = orderLocation.side;
    
    if(side == Side::Buy)
    {
        PooledPriceLevel& priceLevel = bids_.at(price);
        priceLevel.removeSlot(orderLocation.slot);
        if(priceLevel.empty())
        {
            bids_.erase(price);
        }
        
    }
    else
    {
        PooledPriceLevel& priceLevel = asks_.at(price);
        priceLevel.removeSlot(orderLocation.slot);
        if(priceLevel.empty())
        {
            asks_.erase(price);
        }
    }
    
    orderPool_.releaseSlot(orderLocation.slot);
    orderIndex_.erase(iter);
    
}

void PooledLimitOrderBook::displayOrders()
{
    for(const auto item: orderIndex_)
    {
        std::cout << "Id: " << item.first << ", slot: " << item.second.slot << std::endl;
    }
}

std::optional<Price> PooledLimitOrderBook::bestBid() const
{
    if(bids_.empty())
        return std::nullopt;
    
    return bids_.begin()->first;
}
std::optional<Price> PooledLimitOrderBook::bestAsk() const
{
    if(asks_.empty())
        return std::nullopt;
    
    return asks_.begin()->first;
}


std::optional<Quantity> PooledLimitOrderBook::quantityAtPrice(Side side, Price price) const
{
    if(side == Side::Buy)
    {
        auto it = bids_.find(price);
        if(it != bids_.end())
        {
            const PooledPriceLevel& priceLevel = it->second;
            return priceLevel.totalQuantity(orderPool_);
            /*
             more compact approach would be
             return it->second.total_quantity();
             */
            
        }
        return std::nullopt;
    }
    else // (side == Side::Sell)
    {
        auto it = asks_.find(price);
        if(it != asks_.end())
        {
            const PooledPriceLevel& priceLevel = it->second;
            return priceLevel.totalQuantity(orderPool_);
        
        }
        return std::nullopt;
    }

}

std::optional<Price> PooledLimitOrderBook::spread() const
{
    auto bid = bestBid();
    auto ask = bestAsk();
    
    if(!ask.has_value() || !bid.has_value())
        return std::nullopt;
    
    return ask.value() - bid.value();
    
}

void PooledLimitOrderBook::process(const MarketEvent& event)
{
    switch(event.type)
    {
        case EventType::Add:
        {
            const Order order({event.id, event.side, event.price, event.quantity});
            add(order);
            break;
        }
        case EventType::Cancel:
        {
            cancel(event.id);
            break;
        }
        case EventType::Modify:
        {
            modify(event.id, event.quantity);
            break;
            
        }
        default:
            break;
    }
}

bool PooledLimitOrderBook::validate() const
{
    std::size_t orderCount = 0;

    // Validate bid price levels
    for (const auto& [price, priceLevel] : bids_)
    {
        // Empty price levels should never remain in the book.
        if (priceLevel.empty())
        {
            return false;
        }

        for (std::size_t slot : priceLevel.slots())
        {
            // Every referenced slot must contain an active Order.
            const Order* order = orderPool_.get(slot);

            if (order == nullptr)
            {
                return false;
            }

            // Order must belong to this price level.
            if (order->price != price)
            {
                return false;
            }

            // Order must belong to the bid side.
            if (order->side != Side::Buy)
            {
                return false;
            }

            if (order->quantity == 0)
            {
                return false;
            }

            // Every Order in a price level must have an index entry.
            auto indexIt = orderIndex_.find(order->id);

            if (indexIt == orderIndex_.end())
            {
                return false;
            }

            const PooledOrderLocation& location = indexIt->second;

            if (location.side != order->side)
            {
                return false;
            }

            if (location.price != order->price)
            {
                return false;
            }

            if (location.slot != slot)
            {
                return false;
            }

            ++orderCount;
        }
    }

    // Validate ask price levels
    for (const auto& [price, priceLevel] : asks_)
    {
        if (priceLevel.empty())
        {
            return false;
        }

        for (std::size_t slot : priceLevel.slots())
        {
            const Order* order = orderPool_.get(slot);

            if (order == nullptr)
            {
                return false;
            }

            if (order->price != price)
            {
                return false;
            }

            if (order->side != Side::Sell)
            {
                return false;
            }

            if (order->quantity == 0)
            {
                return false;
            }

            auto indexIt = orderIndex_.find(order->id);

            if (indexIt == orderIndex_.end())
            {
                return false;
            }

            const PooledOrderLocation& location = indexIt->second;

            if (location.side != order->side)
            {
                return false;
            }

            if (location.price != order->price)
            {
                return false;
            }

            if (location.slot != slot)
            {
                return false;
            }

            ++orderCount;
        }
    }

    // If every book Order has an index entry, but the index contains
    // additional stale entries, this catches it.
    if (orderCount != orderIndex_.size())
    {
        return false;
    }

    return true;
}
