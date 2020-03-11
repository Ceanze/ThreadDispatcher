# ThreadDispatcher

This is a custom dispatcher using the \<thread\>, \<mutex\> and \<conditional_variable\> libraries.

The dispatcher features three main functions, which are:
  - dispatch
  - wait
  - finished
  
## Dispatch function
This function dispatches work into the work queue which threads in the dispatcher are always checking if there are any work. The work that is being dispatched is a std::function (using the \<functional\> library), which supports lambdas, function pointers, and of course the std::function itself.
When a work is being dispatched a workID will return from the function. This ID can be used with the other two main functions to either wait of check if a work ID is finished.

## Wait function
This function has three overloads, one that waits for _all_ work IDs, one that waits for one specific work ID, and one that waits for several specific work ID. When a wait function is being used the work that was checked will be removed from the internal workDone list, meaning it will not be able to check if this work is finished after a wait has been done on it, since it will be guaranteed done.

## Finished function
This function follows the same principle as the wait function by having three overloads. One overload checks if _all_ work IDs have finished, the second checks if one specific work ID is finished, while the third checks if several specific work IDs are finished. When a work ID is being checked if it is finished it will not be removed from the internal workDone list if the work was not finished. If the work however was finished, then it will be removed. This means that while the work has not finished it is possible to check several times if it has finished, until it is finished.

# How to use
Start by initalizing the ThreadDispatcher and specify how many threads to use.
```c++
ThreadDispatcher::init(NUM_THREADS);
```

Then dispatch any number of work, which can be done at any time.
```c++
ThreadDispatcher::dispatch(WORK[0]);
for (unsigned i = 1; i < WORK_LOOP; i++)
  ThreadDispatcher::dispatch(WORK[i]);
```

If no synchronisaztion or checks are needed then that is all that has to be done. If one want to wait or see if something is finished:
```c++
// Wait for _one_ thread
ThreadDispatcher::wait(WORK[5]);
// Wait for several
ThreadDispatcher::wait({WORK[10], WORK[12]});
// Wait for all
ThreadDispatcher::wait();
```

If one only want to check if it is done, but not wait, then use finished. Returns true if the selected work ID is done:
```c++
// Checks for _one_ thread
ThreadDispatcher::finished(WORK[5]);
// Checks for several
ThreadDispatcher::finished({WORK[10], WORK[12]});
// Checks for all
ThreadDispatcher::finished();
```

When finished with the dispatcher use shutdown to release memory and destroy threads.
```c++
ThreadDispatcher::shutdown();
```

# Example
Can be found in "ThreadTest.cpp"
