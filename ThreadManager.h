#include "Thread.h"
#ifndef THREADMANAGER_H
#define THREADMANAGER_H
// A thread manager class
class ThreadManager
{
        private:
                vector <Thread *> threads; // Vector of Thread pointers
        public:
                ThreadManager();        // Constructor
                void addThread (Thread * p_thread);     // Add a thread
                void start();   // Start all threads 
		void startRR();	// Start all threads Round-Robin
                void barrier(); // Wait for all threads to finish
                ~ThreadManager();       // Destructor
};

#endif