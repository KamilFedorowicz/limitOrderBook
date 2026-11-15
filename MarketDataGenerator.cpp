#include "MarketDataGenerator.h"

MarketEvent MarketDataGenerator::next()
{
    if(activeIds_.empty())
    {
        return generateAdd();
    }
    
    int eventChoice = eventDist_(gen_);
    
    if(eventChoice <=60)
    {
        return generateAdd();
    }
    else if(eventChoice <=85)
    {
        return generateModify();
    }
    else
    {
        return generateCancel();
    }
    
}

MarketEvent MarketDataGenerator::generateAdd()
{
    Price price = static_cast<Price>(priceDist_(gen_));
    Quantity quantity = static_cast<Quantity>(quantityDist_(gen_));
    
    MarketEvent event{EventType::Add, nextOrderId_, nextSide_, price, quantity};
    
    activeIds_.push_back(nextOrderId_);
    
    // advance ID
    nextOrderId_++;
    
    // flip side
    if(nextSide_ == Side::Buy)
    {
        nextSide_ = Side::Sell;
    }
    else
    {
        nextSide_ = Side::Buy;
    }
    
    return event;
}

MarketEvent MarketDataGenerator::generateModify()
{
    std::uniform_int_distribution<size_t> eventDist_{0, activeIds_.size()-1};
    std::size_t eventIndex = eventDist_(gen_);
    OrderId id = activeIds_[eventIndex];
    Quantity quantity = static_cast<Quantity>(quantityDist_(gen_));
    
    MarketEvent event{EventType::Modify, id, Side::Buy, 0, quantity};
    
    return event;
}

MarketEvent MarketDataGenerator::generateCancel()
{
    std::uniform_int_distribution<size_t> eventDist_{0, activeIds_.size()-1};
    std::size_t eventIndex = eventDist_(gen_);
    OrderId id = activeIds_[eventIndex];
    
    MarketEvent event{EventType::Cancel, id, Side::Buy, 0, 0};
     // activeIds_.begin() + eventIndex - iterator to the id to be removed from the list
    activeIds_.erase(activeIds_.begin() + eventIndex);
    
    return event;
}
