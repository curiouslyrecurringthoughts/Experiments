#include <iostream>
#include <random>
#include <experimental/coroutine>

#define USE_MSVC_EXTENSION 0

#if USE_MSVC_EXTENSION

#include <experimental/generator>

using int_generator = std::experimental::generator<int>;

#else

struct int_generator {

	struct promise_type;

	int_generator(int_generator const&) = delete;
	int_generator(int_generator&&) = delete;

	int_generator& operator=(int_generator&&) = delete;
	int_generator& operator=(int_generator const&) = delete;

	//std::coroutine_handle<promise_type> in the future
	using handle = std::experimental::coroutine_handle<promise_type>;

	struct promise_type {
		
		auto get_return_object() {
			return int_generator{ handle::from_promise(*this) };
		}
		
		void return_void() { }
		
		auto yield_value(int value) {
			m_value = value;
			return std::experimental::suspend_always{};
		}

		auto initial_suspend() {
			return std::experimental::suspend_always{};
		}

		auto final_suspend() {
			return std::experimental::suspend_always{};
		}

		int m_value{ 0 };
	};

	int_generator(handle h) : coro{h} {}

	int get() {
		return coro.promise().m_value;
	}

	~int_generator() {
		if (coro)
			coro.destroy();
	}

	struct iterator {

		handle coro;

		iterator& operator++() {
			coro.resume();
			if (coro.done())
				coro = nullptr;
			return *this;
		}

		const int& operator*() const {
			return coro.promise().m_value;
		}

		bool operator==(const iterator& other) const {
			return coro == other.coro;
		}

		bool operator!=(const iterator& other) const {
			return !(operator==(other));
		}

	};

	iterator begin() { iterator it{ coro }; ++it; return it; }
	iterator end() { return iterator{ nullptr }; }

private:
	handle coro;
};

#endif

int_generator rand_numbers(std::size_t num) {

	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<> dis(1, 6);

	for (size_t i = 0; i < num; ++i)
	{
		auto val = dis(gen);
		co_yield val;
	}
}

int main()
{

	auto coro = rand_numbers(10);
	
	for (auto el : coro) {
		std::cout << el << std::endl;
	}

}
