#include <algorithm>
#include <iostream>
#include <numeric>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <array>
#include <new>

template<typename countersType>
void measure() {

	constexpr std::size_t numIter = 40'000'000;
	constexpr std::size_t numExpr = 20;

	struct alignas(64) resultType {
		std::array<std::uint8_t, 64 - sizeof(int)> padding{ 0 };
		countersType cnt;
	};


	int totRes = 0;
	double totalD = 0;

	for (std::size_t q = 0; q < numExpr; ++q) {

		std::vector<resultType> data(numIter);

		const auto start = std::chrono::high_resolution_clock::now();

		for (std::size_t j = 0; j < data.size(); ++j) {
			data[j].cnt.val1 += data[j].cnt.val2;
		}
		const auto stop = std::chrono::high_resolution_clock::now();

		const double duration =
			static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(stop
				- start).count());

		auto fromResTypeToInt = [](const resultType a) {return (a.cnt.val1 + a.cnt.val2) % 10; };
		auto sumAbs10 = [](const int a, const int b) {return (a + b) % 10; };
		auto res = std::transform_reduce(cbegin(data), cend(data), 0, sumAbs10, fromResTypeToInt);

		totRes += res;
		totalD += duration;
	}
	std::cout << "Total duration " << totalD << '\n';
	std::cout << "Result to avoid loop being optimized out " << totRes << '\n';
}

int main() {
	std::cout << "Cache Aligned\n";
	struct alignas(std::hardware_constructive_interference_size) countersAligned {
		int val1{ rand() % 10 };
		int val2{ rand() % 10 };
	};
	measure<countersAligned>();

	std::cout << "Cache NOT aligned\n";
	struct countersNotAligned {
		int val1{ rand() % 10 };
		int val2{ rand() % 10 };
	};
	measure<countersNotAligned>();
}