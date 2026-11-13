#include "basics.h"
#include <iterator>

void Order::print() const
{
    std::cout << "Id: " << id <<
    ", side: " << (side == Side::Buy ? "Buy" : "Sell") <<
    ", price: " << price <<
    ", quantity: " << quantity
    << std::endl;
    
}

PriceLevel::PriceLevel(Price price): price_(price)
{}

std::optional<std::list<Order>::iterator> PriceLevel::add(const Order& order)
{
    if(order.price == price_)
    {
        orders_.push_back(order);
        auto iter = std::prev(orders_.end());
        return iter;
    }
    else
    {
        return std::nullopt;
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

void PriceLevel::printOrders() const
{
    for(const Order& order: orders_)
    {
        order.print();
    }
}

void PriceLevel::remove(std::list<Order>::iterator iter)
{
    orders_.erase(iter);
}

