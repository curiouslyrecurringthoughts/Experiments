// ChronoTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>
#include <iostream>
#include <thread>

//Works: implicit lossless conversion
std::chrono::milliseconds sum1(std::chrono::seconds m1, std::chrono::milliseconds m2) {
	return m1 + m2;
}

#if 0
//Does not work, no implicit lossy conversion
std::chrono::seconds sum2(std::chrono::seconds m1, std::chrono::milliseconds m2) {
	return m1 + m2;
}

//Does not work, no implicit conversion from built-in types
std::chrono::seconds sum3(std::chrono::seconds m1, int m2) {
	return m1 + m2;
}

int main()
{
	std::chrono::microseconds s = sum1(std::chrono::seconds{ 4 }, std::chrono::seconds{ 8 });
	//Last resort, count() to extract the number of milliseconds
	std::cout << s.count() << " ms\n";
}

#endif


#if 0
int main() {

	auto constexpr secondsIn12Hours = 43200;
	using halfDay = std::chrono::duration<float, std::ratio<secondsIn12Hours, 1>>;

	halfDay workedTime{ 1 };
	
	std::chrono::hours sleepedIn{ 8 };

	std::chrono::hours totalTime = std::chrono::duration_cast<std::chrono::hours>(workedTime + sleepedIn);

	std::cout << "Total time used " << totalTime.count() << " hours\n";
}
#endif

int main() {
	
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
	std::this_thread::sleep_until(t1 + std::chrono::seconds{ 2 });
	std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

	std::chrono::steady_clock::duration duration = t2 - t1;

	//auto does_not_work = t1 + t2;

	auto works = duration + std::chrono::steady_clock::now();

	//We need cast because we don't know, in general, the duration type of the clock
	std::cout << "Time elapsed " << 
		std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() 
		<< " ms\n";


	//Two equivalent ways of casting

	//Cast on duration
	std::chrono::system_clock::time_point sysTime = std::chrono::system_clock::now();
	std::cout << "Hours elapsed from 1970 " <<
		std::chrono::duration_cast<std::chrono::hours>(sysTime.time_since_epoch()).count()
		<< '\n';

	//Cast the time point
	auto sysTime2 = std::chrono::time_point_cast<std::chrono::hours>(std::chrono::system_clock::now());
	std::cout << "Hours elapsed from 1970 " <<
		sysTime2.time_since_epoch().count()
		<< '\n';
}