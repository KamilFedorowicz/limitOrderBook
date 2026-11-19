#include "SPSCRingBuffer.h"

bool SPSCRingBuffer::empty() const
{
    return readIndex_ == writeIndex_;
}

bool SPSCRingBuffer::full() const
{
    return (writeIndex_ + 1) % Capacity == readIndex_;
}

bool SPSCRingBuffer::push(const MarketEvent& event)
{
    if(full())
    {
        return false;
    }
    else
    {
        buffer_[writeIndex_] = event;
        writeIndex_ = (writeIndex_ + 1) % Capacity;
        return true;
    }
}

std::optional<MarketEvent> SPSCRingBuffer::pop()
{
    if(empty())
    {
        return std::nullopt;
    }
    else
    {
        MarketEvent event = buffer_[readIndex_];
        readIndex_ = (readIndex_ + 1) % Capacity;
        return event;
    }
}

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
 
 Now lets try to write to it. but the function full() = true, so we cannot write. consequently writeIndex_ = (writeIndex_ + 1) % Capacity; is not executed, and the index remains 4. hence we can distinguish full and empty states. Effectively, the producer is stopped 1 position before writeIndex_ would collide with readIndex_. Hence the usable capacity is Capacity - 1.
*/
