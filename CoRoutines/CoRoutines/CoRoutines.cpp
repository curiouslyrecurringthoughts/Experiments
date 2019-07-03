// CoRoutines.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <mutex>
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include "TSafeQueue.h"

#include <experimental/coroutine>

struct data {
	int sequenceCounter{};
};

std::ostream& operator<<(std::ostream& os, const data& el)
{
	os << " seq: " << el.sequenceCounter;
	return os;
}

int main()
{
	TSafeQueue<data> queue;
	std::mutex printMutex;

	std::thread producer([&queue, &printMutex] (){
		int i = 0;
		while (i < 1000) {
			const data el{i};
			{
				std::lock_guard<std::mutex> guard{ printMutex };
				std::cout << "Produced " << el << '\n';
			}
			queue.push(el);
			std::this_thread::sleep_for(std::chrono::duration<int>(rand() % 3));
			++i;
		}
		queue.done();
	});


	std::thread consumer([&queue, &printMutex ]() {
		while (true) {
			try{
				auto el = queue.pop();
				{
					std::lock_guard<std::mutex> guard{ printMutex };
					std::cout << "Consumed " << el << '\n';
				}
			}
			catch (std::runtime_error& e) {
				std::cout << e.what() << '\n';
				return;
			}
			std::this_thread::sleep_for(std::chrono::duration<int>(rand() % 5));
		}
	});

	producer.join();
	consumer.join();

}