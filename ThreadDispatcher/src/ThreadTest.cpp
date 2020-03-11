#include <iostream>
#include <vector>
#include "ThreadDispatcher.h"

int main()
{
    std::cout << "Main started" << std::endl;

    // initialize the thread dispatcher with the amount of threads it should use
    ThreadDispatcher::init(8);

    // Dispatch 1000 works to be done on the threads and save the workIDs
    std::vector<uint32_t> work(1000);
    for (size_t i = 0; i < 1000; i++)
        work[i] = ThreadDispatcher::dispatch([i]() {/*std::cout << "Dispatch " << i << std::endl;*/ });
    
    // Wait for one specific work ID
    ThreadDispatcher::wait(work[4]);

    // Wait for several specific work IDs
    ThreadDispatcher::wait({1, 25, 500});

    // Check if the specific work ID has finished, does not wait
    if (ThreadDispatcher::finished(work[999]))
        std::cout << "Work 999 has finished!" << std::endl;

    // Check if several specific IDs has finished, does not wait
    if (ThreadDispatcher::finished({ work[5], work[50], work[600] }))
        std::cout << "Work 1, 25, and 500 has finished!" << std::endl;

    if (ThreadDispatcher::finished())
        std::cout << "All work has finished!" << std::endl;

    // Shutdown the dispatcher for a safe exit, wait for all work to finished before that
    ThreadDispatcher::wait();
    ThreadDispatcher::shutdown();

    return 0;
}