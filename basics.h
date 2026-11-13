#pragma once
#include <list>
#include <vector>
#include <cstdint>
#include <iostream>
#include <unordered_map>

enum class Side
{
    Buy,
    Sell
};

using Price = std::int64_t;
using Quantity = std::uint64_t;
using OrderId = std::uint64_t;


struct Order
{
    OrderId id;
    Side side;
    Price price;
    Quantity quantity;
    void print() const;
};

struct OrderLocation
{
    Side side;
    Price price;
    std::list<Order>::iterator it; // stores where the order lives
};

/*
what is price level?
 suppose some orders arrive: (1. 100qty at 99.8; 2. 50 at 99.8; 3. 25 at 99.7; 4. 200 at 99.8). Four different orders, but 3 of them at the same price. Thus the market groups them: Price 99.8, order 1 buy 100, order 2 buy 50, order 4 buy 200. The total quantity available at 99.8 = 350. The whole bucket is one price level. Then, the other bucket (order 3) is at another price level.
 in short price level is the collection of orders ressting at one particular price
 
 */

class PriceLevel
{
public:
    PriceLevel(Price price);
    std::optional<std::list<Order>::iterator> add(const Order& order);
    void remove(std::list<Order>::iterator iterator);
    std::size_t size() const{return orders_.size();}
    Quantity total_quantity() const; // does nto modify the PriceLevel object
    bool empty() const{return orders_.empty();}
    void printOrders() const;
private:
    Price price_;
    std::list<Order> orders_;
};


