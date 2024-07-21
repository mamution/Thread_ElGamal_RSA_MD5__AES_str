#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<iostream>
#include<thread>
#include<mutex>
#include<string>
#include<condition_variable>
#include<queue>
#include<vector>
#include<functional>

using namespace std;

class ThreadPool {
public:
	ThreadPool(int numThreads) :stop(false) {
		for (int i = 0; i < numThreads; i++)
		{
			threads.emplace_back([this] {
				while (true)
				{
					unique_lock<mutex> lock(mtx);
					condition.wait(lock, [this] {
						return !tasks.empty() || stop;
						});
					if (stop && tasks.empty()) {
						return;
					}
					function<void()> task(move(tasks.front()));
					tasks.pop();
					lock.unlock();
					task();
				}
				});
		}
	}
	~ThreadPool() {
		{
			unique_lock<mutex> lock(mtx);
			stop = true;
		}
		condition.notify_all();
		for (auto& i : threads) {
			i.join();
		}
	}
	template<class F, class... Args>
	void enqueue(F&& f, Args&&... args) {
		function<void()> task = bind(forward<F>(f), forward<Args>(args)...);
		{
			unique_lock<mutex> lock(mtx);
			tasks.emplace(move(task));
		}
		condition.notify_one();

	}
private:
	vector<thread> threads;
	queue<function<void()>> tasks;

	mutex mtx;
	condition_variable condition;

	bool stop;
};

#endif
