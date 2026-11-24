#pragma once
#include "basics.h"

struct PooledOrderLocation
{
    Side side;
    Price price;
    std::size_t slot;
};

/*
 in the old design, when the order index was called, the orderIndex map gave an orderLocation struct, which contains an iterator to the node in the list
 
 in the new design, we want to get (instead of the iterator), the slot number into the OrderPool array, not a list. hence a slighty diffrent design of PoolOrderLocation. ultimately, we want a map like std::unordered_map<OrderId, PooledOrderLocation> orderIndex_;
 
 the advantage of using preallocated memort is that elements are close in space. if it is done on heap, then elements are dynamically allocated and they are far away -> more access time.
 
 inside PriceLevels, there was std::list<Order> to which the iterator pointed inside. since list is dynamically allocated, nodes can be scaterred throughout memory, so iterating through orders is a lot of order chasing.
 new design: orders live in the pool std::array<Order, Capacity> orders_; - preallocated storage.
 
 the purpose of this oprimisation is to get control of where the orders are stored. change the list to preallocated orderpool. architecture moving like:
 OrderId
    |
    v
 unordered_map                  still potentially allocating
    |
    v
 PooledOrderLocation
    |
    | slot = 417
    v
 OrderPool
    |
    v
 orders_[417]                   preallocated contiguous storage
 
 Allocation:
 old add: std::list::push_back() - may allocate new node.
 pool add: the storage already exists
 
 in short: we are replacing dynamicslly allocated std::list<Order> storage with preallocated order storage PooledOrder location stores an integer ahndle into that storage.
 */
