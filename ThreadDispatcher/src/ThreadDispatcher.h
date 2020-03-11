#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

class ThreadDispatcher
{
public:
	static void init(uint32_t maxThreads);
	// Dispatch work to a free thread or the next thread that is free
	static uint32_t dispatch(std::function<void()> work);
	// Wait for all
	static void wait();
	// Wait for one specific work ID
	static void wait(uint32_t id);
	// Wait for several specific work IDs
	static void wait(const std::vector<uint32_t>& ids);
	// Returns true if all work is finished. Does not remove ID from work done
	static bool finished();
	// Returns true if the specific work ID is finished. Does not remove ID from work done
	static bool finished(uint32_t id);
	// Returns true if all the specific work IDs are finished. Does not remove IDs from work done
	static bool finished(const std::vector<uint32_t>& ids);
	// Clears all of the finished work
	static void clearFinished();
	static void shutdown();

	// Delete functions to be extra sure
	ThreadDispatcher(const ThreadDispatcher& other) = delete;
	ThreadDispatcher(ThreadDispatcher&& other) = delete;
	ThreadDispatcher& operator=(const ThreadDispatcher& other) = delete;
	ThreadDispatcher& operator=(ThreadDispatcher other) = delete;
	ThreadDispatcher() = delete;
	~ThreadDispatcher() = default;
private:
	static std::queue<std::pair<uint32_t, std::function<void(void)>>> workQueue;
	static std::vector<std::thread> threads;
	static std::mutex mutex;
	static std::condition_variable condVar;
	static bool shouldQuit;
	static std::list<uint32_t> workDone;
	static uint32_t workID;
	static uint32_t workInProgress;

	static void threadLoop();
};