#pragma once
#include "basics.h"

struct PooledOrderLocation
{
    Side side;
    Price price;
    std::size_t slot;
}

/*
 in the old design, when the order index was called, the orderIndex map gave an orderLocation struct, which contains an iterator to the node in the list
 
 in the new design, we want to get (instead of the iterator), the slot number into the OrderPool array, not a list. hence a slighty diffrent design of PoolOrderLocation
 
 */
