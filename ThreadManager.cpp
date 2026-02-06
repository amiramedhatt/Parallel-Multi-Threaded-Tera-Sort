#include "ThreadManager.h"
// Constructor
ThreadManager::ThreadManager()
{
}
// Add a thread
void ThreadManager::addThread (Thread * p_thread)
{
        threads.push_back(p_thread); // Push back thread pointer to threads vector
}
// Start all threads
void ThreadManager::start()
{
        // Loop on all vector threads and start them
        for ( int i = 0 ; i < threads.size();i++)
                threads[i]->start();
}
// Start all threads round robin
void ThreadManager::startRR()
{
	// Loop on all vector threads and start them
	int core_id= 0; // Core index to set the thread to
	for ( int i = 0 ; i < threads.size();i++)
	{
		threads[i]->start(); // Start the thread
		threads[i]->setToCore(core_id); // set it run on the current core_id
		core_id++; // Increment core id for the next thread
		// If core_id is equal to the number of available cores/cpus, the reset it to 0 
		if ( core_id == threads[i]->getCoreCount() ) core_id=0;
	}
}
// Wait on barrier on all threads
void ThreadManager::barrier()
{
        // Loop on all vector thread and block until terminated
        for ( int i = 0 ; i < threads.size();i++)
                threads[i]->waitForRunToFinish();
}
// Destructor
ThreadManager::~ThreadManager()
{
        // Loop on threads vector and delete all threads
        for ( int i = 0 ; i < threads.size();i++)
                delete (threads[i]);
}
