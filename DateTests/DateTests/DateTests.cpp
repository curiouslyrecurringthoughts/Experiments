// DateTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include "date/date.h"

date::year_month_day snapToEndOfMonth(const date::year_month_day& date) {
	if (date.ok()) return date;

	return date.year() / date.month() / date::last;
}

date::year_month_day overflowIntoNextMonth(const date::year_month_day& date) {
	if (date.ok()) return date;

	date::sys_days d{ date };
	return date::year_month_day{ d };
}

int main(){


	//Check that day_point (CppCon 14 talk) and sys_days are the same
	using day_point = std::chrono::time_point<std::chrono::system_clock, date::days>;
	static_assert(std::is_same_v<day_point, date::sys_days>, "NOO");
	
	//Example with number of days
	auto now = std::chrono::system_clock::now();

	date::sys_days nowInDays =
	std::chrono::floor<date::days>(now);

	std::cout << "Days since epoch " << nowInDays.time_since_epoch().count() << '\n';

	//Double as epoch
	using perH = std::chrono::hours::period;
	using hoursInADay = std::ratio_multiply<std::ratio<24>, perH>;
	using daysD = std::chrono::duration<double, hoursInADay>;
	using sys_days_d =
		std::chrono::time_point<std::chrono::system_clock, daysD>;
	
	sys_days_d nowInDoubleDays = now;
	
	std::cout << "Days since epoch (double) " 
				<< nowInDoubleDays.time_since_epoch().count() << '\n';
	
	std::chrono::seconds diff = 
		std::chrono::floor<std::chrono::seconds>
		(nowInDoubleDays - nowInDays);
	
	std::cout << date::time_of_day<std::chrono::seconds>{ diff } << '\n';

	//Date of today
	date::year_month_day ymd{ nowInDays };
	std::cout <<"Today is " << ymd << '\n';


	using namespace date::literals;
	date::year_month_day ymd2 = 2019_y/may/8_d;
	std::cout << "Number of days since epoch " << date::sys_days{ymd2}.time_since_epoch().count()  << '\n';

	//cute syntax, various examples
	date::year_month_day ymd3 = 2019_y / may / 8_d;
	date::year_month_day ymd4 = 8_d/ may / 2019_y;
	date::year_month_day ymd5 = may / 8_d / 2019_y;;

	//day of the week

	date::year_month_weekday ymwd{ nowInDays };
	std::cout << ymwd.weekday() << '\n';

	//last day of the month
	date::year_month_day_last ymd_last = ymwd.year() / ymwd.month() / last;
	std::cout << ymd_last.day() << '\n';

	//last sunday of the month
	date::year_month_weekday_last ymwd_last = ymwd.year()/ ymwd.month()/date::Sunday[last];
	std::cout << date::year_month_day{ ymwd_last } << '\n';

	//Use OK
	date::year_month_day ymd2020 = 2020_y / 8 / last;
	ymd2020 += date::months{1};
	std::cout << ymd2020 << " "<< ymd2020.ok() << std::endl;

	date::year_month_day ymd2020Snap = snapToEndOfMonth(ymd2020);
	std::cout << "Snap to end of month " << ymd2020Snap << '\n';
	date::year_month_day ymd2020Overflow = overflowIntoNextMonth(ymd2020);
	std::cout << "Overflow to next month " << ymd2020Overflow << '\n';

	//constexpr
	constexpr date::year_month_day dyw_last_constexpr = 2019_y / 8 / last;
	constexpr auto has_31_days = dyw_last_constexpr.day() == date::day{ 31 };
	static_assert(has_31_days, "No");

	using namespace std::chrono_literals;

	constexpr auto tp = date::sys_days{ 1970_y / 1 / 3_d } + 7h;
	static_assert(tp.time_since_epoch() == std::chrono::hours{ 7 + 24 * 2 }, "NO2");
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
