#pragma  once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

std::mutex mtx;
std::condition_variable condProd;
std::condition_variable condCons;
std::vector<int> nums{};
std::vector<int> numsToAdd{1,2};
size_t numsPrinted = 0;
bool producerFinished = false;

void myProducer()
{
    for(size_t i = 0; i<numsToAdd.size(); i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Producer owns the mutex\n";
        
        condProd.wait(lock, [&]
                      {
            
            bool condition = numsPrinted == nums.size();
            
            std::cout << "Producer tries to acquire the mutex\n";
            if(condition)
            {
                std::cout << "Producer acquired the mutex\n";
            }
            else
            {
                std::cout << "Producer did not acquire the mutex\n";
            }
            
            return condition;
        });
        
        nums.push_back(numsToAdd[i]);
        std::cout << "Producer added number " << numsToAdd[i] << " to the vector\n";
        std::cout << "Producer unlocking the mutex\n";
        lock.unlock();
        
        condCons.notify_one();
    }
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        producerFinished = true;
    }
    condCons.notify_one();

    
}

void myConsumer()
{
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    while(true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Consumer owns the mutex\n";
        
        condCons.wait(lock, [&]
                      {
            
            bool condition = numsPrinted < nums.size() || producerFinished;
            
            if(condition)
            {
                std::cout << "Consumer condition is satisfied\n";
            }
            else
            {
                std::cout << "Consumer condition is not satisfied\n";
            }
            return condition;
        });
        
        
        while(numsPrinted < nums.size())
        {
            std::cout << "Consumer processed number: " << nums[numsPrinted] << std::endl;
            numsPrinted++;
        }
        
        if(numsPrinted == numsToAdd.size() && producerFinished)
        {
            break;
        }
        
        condProd.notify_one();
        
        

        
    }
    
}

void runTests()
{
    std::thread t1(myProducer);
    std::thread t2(myConsumer);
    
    t1.join();
    t2.join();
}

/*
 conditionVariable.wait(lock, predicate) roughly behaves like:
 while (!predicate())
 {
     // unlock mutex
     // sleep
     // wake up
     // acquire mutex again
 }
 
 condition variables let producer and consumer sleep efficiently when they cannot make progress
 */
