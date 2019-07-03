// StringView.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <string_view>

std::string randomString(const std::size_t length)
{
	std::stringstream ss;
	const std::string alph = "abcdefghilmonpqrstuvzjkxwy";
	const size_t numLetters = alph.length();
	for (std::size_t i = 0; i < length; ++i) {
	
		ss << alph[rand() % numLetters];
	}

	return ss.str();
}

struct Employee {
	std::string id{randomString(40)};

	std::string_view getId() const {
		if (rand() % 6 > 3)
			return id;
		return "POLETNAJFOEJDCJEKJDOEJDOEJMDIOEDIONEOIDFEIO";
	}

	std::string_view getId2() const {
		const auto val = (rand() % 6) < 7 ? id : "POLETNAJFOEJDCJEKJDOEJDOEJMDIOEDIONEOIDFEIO";
		static_assert(std::is_same_v<decltype(val), const std::string>, "NOJFIEJ");
		return val;
	}

};

int main()
{
	Employee emp;
	for (int i = 0; i < 10; ++i)
		std::cout << emp.getId2() << '\n';
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
