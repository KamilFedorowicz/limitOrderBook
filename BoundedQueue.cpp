#include "BoundedQueue.h"

BoundedQueue::BoundedQueue(std::size_t capacity): capacity_(capacity)
{}

void BoundedQueue::add(const MarketEvent& event)
{
    std::unique_lock<std::mutex> lock(mutex_);
    
    // when the producer reaches this line, it already wns the mutex
    // if the predicate (statement inside condition variable) is true, then the mutex stays locked. if it is false, then it unlocks the mutex, and go to slee. once it is notified, it reacquires mutex and checks the precicate again
    notFull_.wait(lock, [this]
                  {
        return queue_.size() < capacity_;
    });
    
    queue_.push_back(event);
    notEmpty_.notify_one(); // wake one thread currently waiting on notEmpty
    // the notification makes it eligible to wake, but before wait() can return, the mutex must be reacquired

}

MarketEvent BoundedQueue::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    
    // sleep until i am notified and queue_ is not empty
    notEmpty_.wait(lock, [this]
                   {
        // this is predicate - answers: is the condition im waiting for actually true
        // when the condition is true, no more waiting
        return !queue_.empty();
    });
    
    MarketEvent event = queue_.front();
    queue_.pop_front();
    notFull_.notify_one();
    
    return event;
}

std::size_t BoundedQueue::size()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}
