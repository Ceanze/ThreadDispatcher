#include "ThreadDispatcher.h"

#include <iostream>

std::vector<std::thread> ThreadDispatcher::threads;
std::queue<std::pair<uint32_t, std::function<void(void)>>> ThreadDispatcher::workQueue;
std::condition_variable ThreadDispatcher::condVar;
std::mutex ThreadDispatcher::mutex;
bool ThreadDispatcher::shouldQuit = false;
std::list<uint32_t> ThreadDispatcher::workDone;
uint32_t ThreadDispatcher::workID;
uint32_t ThreadDispatcher::workInProgress;

void ThreadDispatcher::init(uint32_t maxThreads)
{
	std::cout << "Init ThreadDispatcher" << std::endl;
	workInProgress = 0;

	threads.resize(maxThreads);
	for (size_t i = 0; i < maxThreads; i++)
		threads[i] = std::thread(ThreadDispatcher::threadLoop);
}

uint32_t ThreadDispatcher::dispatch(std::function<void(void)> work)
{
	// Be the only one who touches the queue when dispatching
	std::lock_guard<std::mutex> lock(mutex);
	uint32_t ID = workID++;
	workInProgress++;
	workQueue.push({ ID, work });
	condVar.notify_one();
	return ID;
}

void ThreadDispatcher::wait()
{
	while (!workQueue.empty());
}

void ThreadDispatcher::wait(uint32_t id)
{
	while (true) {
		auto it = std::find(workDone.begin(), workDone.end(), id);
		if (it != workDone.end())
			break;
	}
	// TODO: Mutex to remove safely?
	workDone.remove(id);
}

void ThreadDispatcher::wait(const std::vector<uint32_t>& ids)
{
	for (size_t i = 0; i < ids.size(); i++) {
		while (true) {
			auto it = std::find(workDone.begin(), workDone.end(), ids[i]);
			if (it != workDone.end())
				break;
		}
		// TODO: Mutex to remove safely?
		workDone.remove(ids[i]);
	}
}

bool ThreadDispatcher::finished()
{
	return workInProgress == 0;
}

bool ThreadDispatcher::finished(uint32_t id)
{
	auto it = std::find(workDone.begin(), workDone.end(), id);
	if (it != workDone.end())
		return true;
	return false;
}

bool ThreadDispatcher::finished(const std::vector<uint32_t>& ids)
{
	for (size_t i = 0; i < ids.size(); i++) {
		auto it = std::find(workDone.begin(), workDone.end(), ids[i]);
		if (it == workDone.end())
			return false;
	}
	return true;
}

void ThreadDispatcher::clearFinished()
{
	std::lock_guard<std::mutex> lock(mutex);
	workDone.clear();
}

void ThreadDispatcher::shutdown()
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		shouldQuit = true;
		condVar.notify_all();
	}

	// Wait for threads to finish before exit
	for (size_t i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
		{
			printf("Destructor: Joining thread %zu until completion\n", i);
			threads[i].join();
		}
	}

	clearFinished();
}

void ThreadDispatcher::threadLoop()
{
	std::unique_lock<std::mutex> lock(mutex);

	do {
		// Wait until we have data or a quit signal
		condVar.wait(lock, [&] {
			return (workQueue.size() || shouldQuit);
		});

		// after wait, we own the lock
		if (!shouldQuit && workQueue.size())
		{
			auto op = std::move(workQueue.front());
			workQueue.pop();

			// unlock now that we're done messing with the queue
			lock.unlock();

			op.second();

			lock.lock();
			workDone.push_back(op.first);
			workInProgress--;
		}
	} while (!shouldQuit);
}
