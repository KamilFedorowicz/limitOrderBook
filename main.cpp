#include <iostream>

#include "basics.h"
#include "LimitOrderBook.h"
#include "tests.cpp"
#include "MarketDataGenerator.h"
#include "BoundedQueue.h"
#include <thread>
#include <functional>
#include <chrono>
#include "learningCondVars.h"
#include "SPSCRingBuffer.h"

constexpr std::size_t eventCount = 1'000'000;

void producer(MarketDataGenerator& generator, BoundedQueue& queue)
{
    for(size_t i = 0; i < eventCount; i++)
    {
        MarketEvent event = generator.next();
        queue.add(event);
    }
}

void consumer(LimitOrderBook& book, BoundedQueue& queue)
{
    for(std::size_t i=0; i<eventCount; i++)
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        /*
         since we are using queue.pop(), it will wait in case the queue is empty
         when it returns, the event is guaranted to be a valid market event
         */
        auto event = queue.pop();
        book.process(event);
    }
}

double runSingleThreaded()
{
    LimitOrderBook book;
    MarketDataGenerator generator;

    auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < eventCount; ++i)
    {
        MarketEvent event = generator.next();
        book.process(event);
    }

    auto end = std::chrono::steady_clock::now();

    // use std::chrono::duration<double, std::muli> ms = duration; for milisedonds
    double seconds = std::chrono::duration<double>(end - start).count();

    double throughput = eventCount / seconds;
    
    std::cout << "Single threaded:\n";
    std::cout << "Events: " << eventCount << std::endl;
    std::cout << "Time: " << seconds << "s" << std::endl;
    std::cout << "Throughput: " << throughput << " events/s" << std::endl;
    
    return seconds;
}

double runMultiThreaded()
{
    LimitOrderBook book;
    MarketDataGenerator generator;
    BoundedQueue queue(100);
    
    auto start = std::chrono::steady_clock::now();
    
    // std::ref(...) forces to pass objects by reference instead of coppying them
    std::thread producerThread(producer, std::ref(generator), std::ref(queue));
    std::thread consumerThread(consumer, std::ref(book), std::ref(queue));
    
    producerThread.join();
    consumerThread.join();
    
    auto end = std::chrono::steady_clock::now();
    
    double seconds = std::chrono::duration<double>(end - start).count();

    double throughput = eventCount / seconds;
    
    std::cout << "Multi threaded:\n";
    std::cout << "Events: " << eventCount << std::endl;
    std::cout << "Time: " << seconds << "s" << std::endl;
    std::cout << "Throughput: " << throughput << " events/s" << std::endl;
    
    return seconds;
}

int main(int argc, const char * argv[]) {

    SPSCRingBuffer buffer;
    MarketDataGenerator generator;
    
    for(std::size_t i = 0; i < 10; i++)
    {
        MarketEvent event = generator.next();
        buffer.push(event);
    }
    
    
    
    /*
    double timeSingle = 0;
    double timeMulti = 0;
    for(int i=0; i<2; i++)
    {
        timeSingle += runSingleThreaded();
        timeMulti += runMultiThreaded();
    }

    std::cout << "Single: " << timeSingle/10.0 << ", time multi: " << timeMulti/10.0 << std::endl;
    
    // assert(book.validate());
    */
    
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
