#include <iostream>

#include "basics.h"
#include "LimitOrderBook.h"

int main(int argc, const char * argv[]) {

    Order order1{123, Side::Buy, 9800, 100};
    Order order2{124, Side::Sell, 9801, 200};
    Order order3{125, Side::Buy, 9800, 400};
    Order order4{126, Side::Buy, 10000, 150};
    Order order5{127, Side::Sell, 10200, 260};
    Order order6{128, Side::Buy, 1900, 404};
    
    
    LimitOrderBook limitOrderBook;
    limitOrderBook.add(order1);
    limitOrderBook.add(order2);
    limitOrderBook.add(order3);
    limitOrderBook.add(order4);
    limitOrderBook.add(order5);
    limitOrderBook.add(order6);
    
    limitOrderBook.cancel(123);
    
    std::cout << "Asks: " << std::endl;
    limitOrderBook.printAsks();
    
    std::cout << "Bids: " << std::endl;
    limitOrderBook.printBids();
    
    
    
    return 0;
}

/*
 bid - order to buy
 ask - order to sell
 
 LOB - limited order book
 participanys may say 'i want to buy 100 units at 9 each' or 'sell 40 units at 100'
 orders form a limit book.
 SELL ORDERS / ASKS

Price        Quantity
---------------------
100.30          120
100.20           40
100.10           80    <- best ask
=====================
spread = 0.20
=====================
99.90          150    <- best bid
99.80          200
99.70           75

 BUY ORDERS / BIDS
 essentially buyers want to little as possible, sellers want to get as much as possible
 the most competitive buy - highest bid, most competitive sell - lowest bid (order to buy)
 
 for a simplified system, order may containt id, side (buy/sell), price, quantity.
 
 there can be many orders at the same price:
 id=101 BUY 100 @ 99.80
 id=102 BUY  30 @ 99.80
 id=103 BUY  70 @ 99.80
 this gives the total quantity of 200.
 
 Incoming market data is treated as a sequence of events
 ADD     101 BUY  100 @ 99.80
 ADD     102 BUY   50 @ 99.80
 ADD     103 SELL  75 @ 100.10
 CANCEL  102
 MODIFY  101 quantity=60
 
 ADD creates new order. book must determine the side, locate the price level, insert order, make it possiblr to find the order id later
 CANCEl removes existing order. e.g. CANCEL id = 101 -> how do we find order 101 without scanning entire book - we want a oncstant time id lookup
 MODIFY - (we make it initially restricted to change the quantity to buy). changing the buy price will need to change the price order which is complicated for now. This will be introduced later.
 Exchanges use price/time priority: better price first. At the same price, earlier order first.
 Prices are represented as integer tick. 1tick = 0.01, e.g. 99.80 = 9980 ticks
 
 */
