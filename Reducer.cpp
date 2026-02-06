#include "TeraSort.h"
#include "Thread.h"
#include "ThreadManager.h"

Reducer::Reducer(ShuffleBuffer *p_shuffleBuffer, string algo) : Thread()
{
    this->shuffleBuffer = p_shuffleBuffer;
    this->algo = algo;
}

// FIX: Don't delete the shuffle buffer (it's managed by ShuffleBuffers)
Reducer::~Reducer()
{
    // ShuffleBuffer is managed by ShuffleBuffers class, don't delete here
    shuffleBuffer = NULL;
}
