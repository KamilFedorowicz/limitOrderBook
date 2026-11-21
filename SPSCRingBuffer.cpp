#include "SPSCRingBuffer.h"

bool SPSCRingBuffer::empty() const
{
    return readIndex_ == writeIndex_;
}

bool SPSCRingBuffer::full() const
{
    return (writeIndex_ + 1) % Capacity == readIndex_;
}

// during push the consumer suddenly wont modify writeIndex_
// memory_order_acquire is less strict memory ordering than the default one
bool SPSCRingBuffer::push(const MarketEvent& event)
{
    // load creates an ordinary local variable from the atomic
    
    // producer: where did I leave off (note that writeIndex_ is modified by producer only)
    std::size_t write = writeIndex_.load(std::memory_order_relaxed);
    std::size_t read = readIndex_.load(std::memory_order_acquire);
    
    std::size_t nextWrite = (write + 1) % Capacity;
    
    if(nextWrite == read)
    {
        return false;
    }
    else
    {
        // write the actual event, publish that the event is available
        buffer_[write] = event; // write
        
        // tell other thread that the data is ready
        writeIndex_.store(nextWrite, std::memory_order_release); // publish new write position
        // put the package in the mailbox, raise a flag saying there is a mail (second line)
        // everything this thread did before release operation must be published
        // before another thread successfully acquires this publication
        return true;
    }
}

// during pop the producer wont modify read index
std::optional<MarketEvent> SPSCRingBuffer::pop()
{
    // memory_order_relaxed means - i need this access to be atomic but i dont
    // need this access to establish order of memory operations between threads
    // basically asking 'where did i leave it last time' because only
    // consumer modifies this variable
    std::size_t read = readIndex_.load(std::memory_order_relaxed);
    
    // writeIndex_ i a different story because it was written by the producer
    // (push). this value communicates 'i finished the event, you can read it'
    // producer says 'event xyz' is ready. consumer 'i observed that its ready'
    // the index isnt that important, it is the market data that is
    std::size_t write = writeIndex_.load(std::memory_order_acquire);
    
    if(write == read)
    {
        return std::nullopt;
    }
    else
    {
        // we need a guarantee: if the consumer sees the updated writeIndex_,
        // then it must also see everything that was written to the buffer before
        MarketEvent event = buffer_[read]; // reading actual data
        std::size_t nextRead = (read + 1) % Capacity;
        
        // consumer communicating in another direction.
        // it says to producer: 'finished reading the slot, you can reuase it'
        readIndex_.store(nextRead, std::memory_order_release);
        return event;
    }
}

// note that pop reads write and modifies read and vice versa

/*
readIndex_, writeIndex_ is where the NEXT read/write happens.
 [A][B][C][D][ ]
  R              W

 read  = 0
 write = 4
 
the above state is after 4 elements.
 
 Read one element:
 [x][B][C][D][ ]
     R           W

 read  = 1
 write = 4
 
 then read B, C, D:
 [x][x][x][x][ ]
               R/W

 read = 4
 write = 4
 
 now readIndex_ == writeIndex_, and hence the buffer is empty.
 
 Then we add 1 more element:
            R
            ↓
[x][x][x][x][E]
^
W
 
 read  = 4
 write = 0

Think of this:
 R            W
 ↓            ↓
[A][B][C][D][ ]

read  = 0
write = 4
 
 Now lets try to write to it. but the function full() = true, so we cannot write.
 consequently writeIndex_ = (writeIndex_ + 1) % Capacity; is not executed, and
 the index remains 4. hence we can distinguish full and empty states. Effectively,
 the producer is stopped 1 position before writeIndex_ would collide with readIndex_.
 Hence the usable capacity is Capacity - 1.
 
 My index:
     load relaxed

 Other thread's index:
     load acquire

 Publishing my progress:
     store release
 
 A
 |
 release store
        |
        | synchronizes-with
        v
                 acquire load
                     |
                     v
                     D
*/
