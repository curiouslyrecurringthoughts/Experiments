// CoRoutinesExampleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <future>
#include <queue>
#include <utility>
#include <experimental/coroutine>

class Worker {

public:

	static Worker& getInstance() {
		static Worker worker;
		return worker;
	}

	using coroHandle = std::experimental::coroutine_handle<>;

	void suspend(coroHandle handle, int inputData) {
		m_suspendedCoros.push({handle, inputData});
	}

	int veryComplexDataGeneration(int inputData) { return std::rand() % 10 + inputData; }

	void run() {
		while (!m_suspendedCoros.empty()) {
			auto [coro, inputData]= m_suspendedCoros.front();
			m_suspendedCoros.pop();
			outputData = veryComplexDataGeneration(inputData);
			coro.resume();
		}
	}

	int getData() const { return outputData; }

private:

	std::queue<std::pair<coroHandle, int>> m_suspendedCoros;
	int outputData{ -1 };
};

class Awaiter {
public:
	
	Awaiter(int inData) : inData(inData) {}

	bool await_ready()
	{
		return false;
	}

	void await_suspend(std::experimental::coroutine_handle<> handle)
	{
		Worker::getInstance().suspend(handle, inData);
	}

	int await_resume()
	{
		return Worker::getInstance().getData();
	}

private:
	int inData;
};

std::future<int> coRoutine(std::string name){

	int sum = 0;
	while (sum < 20) {
		int input_data = rand() % 5;
		const auto s = co_await Awaiter{input_data};
		std::cout << name << " received " << s << '\n';
		sum += s;	
	}
	co_return sum;
}


int main()
{	
	auto f1 = coRoutine("c1");
	auto f2 = coRoutine("c2");
	auto f3 = coRoutine("c3");
	
	Worker::getInstance().run();

	std::cout << "RESULTS " << f1.get() << " " << f2.get() << " " << f3.get() << '\n';
}
