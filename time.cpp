
#pragma once

#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <map>
#include <unordered_map>

#include "JSON/include/JSON.hpp"

const uint64_t YEAR_ZERO = 1900;
const uint64_t YEAR_MAX = 2100;

bool is_leap(uint64_t year) {
	if((year%400==0) || (year%4==0 && year%100!=0))
		return true;
	return false;
}

uint64_t days_in_year(uint64_t year) {
	if((year%400==0) || (year%4==0 && year%100!=0))
		return 366;
	return 365;
}

uint64_t days_in_month(uint64_t month, uint64_t year) {
	if(month == 2) {
		if(days_in_year(year) == 366)
			return 29;
		return 28;
	}
	if(month&1)
		return 31;
	return 30;
}

uint64_t days_to_year(uint64_t year) {
	if(year > YEAR_MAX)
		exit(311);
	uint64_t sum = ((365*4 + 1) * ((year-YEAR_ZERO)/4))-(year>2000 ? 1 : 0);
	for(uint64_t i=year-(year%4); i<year; ++i)
		sum += days_in_year(i);
	return sum;
}

uint64_t year_from_days(uint64_t days, uint64_t& not_even) {
	uint64_t years = YEAR_ZERO + 4*(days/(365*4+1));
	days %= (365*4+1);
	days += (years>2000 ? 1 : 0);
	for(uint64_t i=years;; ++i) {
		const uint64_t d = days_in_year(i);
		if(days <= d) {
			not_even = days;
			return i;
		}
		days -= d;
	}
}

uint64_t __month_from_days(uint64_t days, uint64_t& month_days, uint64_t year) {
	days--;
	for(uint64_t i=1;; ++i) {
		const uint64_t d = days_in_month(i, year);
		if(days <= d) {
			month_days = days+1;
			return i;
		}
		days -= d;
	}
}

static uint64_t _leap_month[366], _normal_month[365];
static uint64_t _leap_days[366], _normal_days[365];
uint64_t month_from_days(uint64_t days, uint64_t& month_days, uint64_t year) {
	static int __static = 1;
	if(__static) {
		__static = 0;
		for(int i=0; i<366; ++i) {
			_leap_month[i] = __month_from_days(i+1, _leap_days[i], 2000);
			if(i<365)
				_normal_month[i] = __month_from_days(i+1, _normal_days[i], 2001);
		}
	}
	if(is_leap(year)) {
		month_days = _leap_days[days-1];
		return _leap_month[days-1];
	} else {
		month_days = _normal_days[days-1];
		return _normal_month[days-1];
	}
}

uint64_t days_in_month_day(uint64_t month, uint64_t days_in_month,
		uint64_t year) {
	uint64_t sum = days_in_month;
	for(uint64_t i=1; i<month; ++i) {
		sum += ::days_in_month(i, year);
	}
	return sum;
}

struct Time {
	inline Time() {seconds=0;}
	inline Time(uint64_t s) { seconds=s; }
	inline Time(uint64_t years, uint64_t months, uint64_t days, uint64_t hours,
			uint64_t minutes, uint64_t seconds) {
		this->seconds =
			day(
					days_to_year(years) +
					days_in_month_day(months, days, years)
					) +
			hour(hours) +
			minute(minutes) +
			Time(seconds);
	}
	inline static Time minute(uint64_t n) { return Time(n*60llu); }
	inline static Time hour(uint64_t n) { return Time(n*3600llu); }
	inline static Time day(uint64_t n) { return Time(n*3600llu*24llu); }
	
	inline uint64_t years() const { uint64_t tmp;return year_from_days(days(), tmp); }
	inline uint64_t years(uint64_t& tmp) const { return year_from_days(days(), tmp); }
	inline uint64_t days() const { return seconds / day(1).seconds; }
	inline uint64_t hours() const { return seconds / hour(1).seconds; }
	inline uint64_t minutes() const { return seconds / minute(1).seconds; }
	inline uint64_t hours_in_day() const { return hours() % 24; }
	inline uint64_t minutes_in_day() const { return minutes() % 60; }
	inline uint64_t seconds_in_day() const { return seconds % 60; }
	
	inline Time  operator+ (Time o) const { return Time(seconds+o.seconds); }
	inline Time& operator+=(Time o)       { seconds+=o.seconds; return *this; }
	inline Time  operator- (Time o) const { return Time(seconds-o.seconds); }
	inline Time& operator-=(Time o)       { seconds-=o.seconds; return *this; }
	inline Time  operator% (Time o) const { return Time(seconds%o.seconds); }
	inline Time& operator%=(Time o)       { seconds%=o.seconds; return *this; }
	
	inline operator uint64_t() const { return seconds; }
	
	inline bool operator< (Time o) const { return seconds <  o.seconds; }
	inline bool operator<=(Time o) const { return seconds <= o.seconds; }
	inline bool operator> (Time o) const { return seconds >  o.seconds; }
	inline bool operator>=(Time o) const { return seconds >= o.seconds; }
	
	inline Time rounded_down_to_full_days() const {
		return Time(seconds - (seconds%day(1).seconds));
	}
	
	inline void decompose(uint64_t& years, uint64_t& months, uint64_t& days,
			uint64_t& hours, uint64_t& minutes, uint64_t& seconds) const {
		uint64_t rest_days;
		years = this->years(rest_days);
		months = month_from_days(rest_days, days, years);
		hours = hours_in_day();
		minutes = minutes_in_day();
		seconds = seconds_in_day();
	}
	
	inline std::string to_string() const {
		uint64_t years, months, days, hours, minutes, seconds;
		decompose(years, months, days, hours, minutes, seconds);
		char buf[64];
		sprintf(buf, "%4.4lu-%2.2lu-%2.2lu %2.2lu:%2.2lu:%2.2lu",
				years, months, days, hours, minutes, seconds);
		return std::string(buf);
	}
	
	uint64_t seconds;
};

Time get_tm(const JSON& json) {
	return Time(
			json["year"].Integer(),
			json["month"].Integer(),
			json["day"].Integer(),
			json["hour"].Integer(),
			json["minute"].Integer(),
			json["second"].Integer() );
}

Time minute(int minutes) {
	return Time::minute(minutes);
}

Time hour(int hours) {
	return Time::hour(hours);
}

Time day(int days) {
	return Time::day(days);
}

std::ostream& operator<<(std::ostream& stream, Time t) {
	return stream << t.to_string();
}

inline std::string to_string(Time t) {
	return t.to_string();
}

bool is_weekend(Time t) {
	uint64_t day = t.days() % 7;
	return day==2 || day==3;
}

