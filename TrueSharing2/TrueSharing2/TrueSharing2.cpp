#include <algorithm>
#include <iostream>
#include <numeric>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <array>
#include <new>


template<typename resultType>
void measure() {

	constexpr std::size_t numIter = 40'000'000;
	constexpr std::size_t numExpr = 20;


	std::cout << "int size " << sizeof(int) << '\n';
	std::cout << "resultType size " << sizeof(resultType) << '\n';

	int totRes = 0;
	double totalD = 0;

	for (std::size_t q = 0; q < numExpr; ++q) {

		std::vector<resultType> data(numIter);
		int ret = 0;

		const auto start = std::chrono::high_resolution_clock::now();

		for (std::size_t j = 0; j < data.size(); ++j) {
			ret = (ret + data[j].val1 + data[j].val2) % 10;
		}
		const auto stop = std::chrono::high_resolution_clock::now();

		const double duration =
			static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(stop
				- start).count());

		totRes += ret;
		totalD += duration;
	}
	std::cout << "Total duration " << totalD << '\n';
	std::cout << "Result to avoid loop being optimized out " << totRes << '\n';
}

int main() {

	std::cout << "Cache Aligned\n";

	struct alignas(64) resultTypeAligned {
		int val1{ rand() % 10 };
		int val2{ rand() % 10 };
		std::array<std::uint8_t, 64 - sizeof(int)> padding{ 0 };
	};
	measure<resultTypeAligned>();

	std::cout << "Cache NOT aligned\n";
	struct alignas(64) resultTypeNotAligned {
		int val1{ rand() % 10 };
		std::array<std::uint8_t, 64 - sizeof(int)> padding{ 0 };
		int val2{ rand() % 10 };
	};
	measure<resultTypeNotAligned>();

}