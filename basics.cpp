#include "basics.h"


PriceLevel::PriceLevel(Price price): price_(price)
{}

void PriceLevel::add(const Order& order)
{
    if(order.price == price_)
    {
        orders_.push_back(order);
    }
    else
    {
        std::cout << "Inconsistent price levels for price " << order.price << "\n";
    }
}

Quantity PriceLevel::total_quantity() const
{
    Quantity result = 0;
    
    for(const Order& order: orders_)
    {
        result += order.quantity;
    }
    
    return result;
}

