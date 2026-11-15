#pragma once
#include "basics.h"
#include <random>


class MarketDataGenerator
{
public:
    MarketEvent next();
    MarketEvent generateAdd();
    MarketEvent generateModify();
    MarketEvent generateCancel();
    
private:
    OrderId nextOrderId_ = 1;
    Side nextSide_ = Side::Buy;

    std::mt19937 gen_{42};

    std::uniform_int_distribution<int> priceDist_{9500, 10500};
    std::uniform_int_distribution<int> quantityDist_{1, 1000};
    std::uniform_int_distribution<int> eventDist_{1, 100};
    
    std::vector<OrderId> activeIds_;
};
