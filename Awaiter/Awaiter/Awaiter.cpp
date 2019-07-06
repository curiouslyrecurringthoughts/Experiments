// GeneratorCoRoutines.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <thread>
#include <future>
#include <iostream>
#include <experimental/coroutine>

class Waiter {
public:
	//Neede by Awaitable concept
	bool await_ready();
	void await_suspend(std::experimental::coroutine_handle<>);
	void await_resume();

	//My stuff
	void SetReady();
private:
	std::experimental::coroutine_handle<> m_handle;
};

void Waiter::SetReady()
{
	std::cout << "SET READY!\n";
	if (m_handle)
		m_handle.resume();
}

bool Waiter::await_ready()
{
	return false;
}

void Waiter::await_suspend(std::experimental::coroutine_handle<> handle)
{
	m_handle = std::move(handle);
}

void Waiter::await_resume()
{
	std::cout << "I am DONE Waiting! \n";
}

using task = std::future<void>;

/*
struct task {
	struct promise_type;

	using handle = std::experimental::coroutine_handle<promise_type>;

	struct promise_type {
		auto get_return_object() { return task{ handle::from_promise(*this) }; }
		void return_void() { }
		void result() {}
		auto initial_suspend() { return std::experimental::suspend_never{}; }
		auto final_suspend() { return std::experimental::suspend_never{}; }
	};

	task(handle h) : coro(h) {}

	void get() { coro.promise().return_void(); }

private:
	handle coro;
};
*/

Waiter waiter;

task func() {

	std::cout << "Func()\n";

	std::cout << "about to await!\n";

	co_await waiter;

	std::cout << "co await passed!\n";

}

int main()
{
	std::thread t
	{
		[]()
		{
			std::cout << "About to set sleep\n";
			std::this_thread::sleep_for(std::chrono::duration<int>(3));
			std::cout << "About to set ready\n";
			waiter.SetReady();
		}
	};


	std::cout << "About to func()\n";
	auto a = func();
	a.get();

	t.join();
	std::cout << "after join!\n";

}

