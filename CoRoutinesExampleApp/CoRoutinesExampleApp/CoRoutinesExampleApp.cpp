// CoRoutinesExampleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <thread>
#include <iostream>
#include <string>
#include <future>
#include <experimental/coroutine>
#include <experimental/generator>

#include "WorkerQueue.h"

template<typename T>
class QueueAwaiter {
public:
	
	QueueAwaiter(WorkerQueue<T>& queue) : m_queue{ queue } { }

	//Needed by Awaiter concept
	bool await_ready()
	{
		return !m_queue.empty();
	}

	void await_suspend(std::experimental::coroutine_handle<> handle)
	{
		m_queue.wakeMeUp(handle);
	}

	T await_resume()
	{
		return  m_queue.pop();
	}

private:
	WorkerQueue<T>& m_queue;
};

std::future<void> consumer(WorkerQueue<std::string>& queue) {
	while (true) {
		QueueAwaiter<std::string> aw{ queue };
		std::string result = co_await aw;
		std::cout << "Returned " << result << '\n';
	}
}

int main()
{
	WorkerQueue <std::string> queue;

	std::thread producer{
		[&queue]() {
			unsigned int i = 0;
			while(true){
				std::string s = "HELLO" + std::to_string(i++);
				std::cout << "Produced " << s << '\n';
				queue.push(std::move(s));
			}
		}
	};

	producer.detach();

	auto f = consumer(queue);
	f.get();
}
