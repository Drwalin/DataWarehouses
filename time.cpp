
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

uint64_t days_in_year(uint64_t year) {
	if((year%400==0) || (year%4==0 && year%100!=0))
		return 366;
	return 365;
}

int days_in_month(int month, int year) {
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
	uint64_t sum = 1;
	for(uint64_t i=1900; i<year; ++i)
		sum += days_in_year(i);
	return sum;
}

uint64_t year_from_days(uint64_t days, uint64_t& not_even) {
	for(uint64_t i=1900;; ++i) {
		const uint64_t d = days_in_year(i);
		if(days < d) {
			not_even = days;
			return i;
		}
		days -= d;
	}
}

int month(int days, int& mon_days, int year) {
	for(int i=1;; ++i) {
		const int d = days_in_month(i, year);
		if(days < d) {
			mon_days = days;
			return i;
		}
		days -= d;
	}
}

struct Time {
	inline Time() {seconds=0;}
	inline Time(uint64_t s) { seconds=s; }
	inline Time(tm t) {
		seconds =
			(uint64_t)t.tm_sec
			+ (uint64_t)(t.tm_min*60llu)
			+ (uint64_t)(t.tm_hour*3600ll)
			+ (uint64_t)(t.tm_yday*3600*24llu)
			+ (uint64_t)(days_to_year(t.tm_year+1900)*3600llu*24llu);
	}
	inline static Time minute(uint64_t n) { return Time(n*60llu); }
	inline static Time hour(uint64_t n) { return Time(n*3600llu); }
	inline static Time day(uint64_t n) { return Time(n*3600llu*24llu); }
	
	inline uint64_t years() const { uint64_t tmp;return year_from_days(days(), tmp); }
	inline uint64_t days() const { return seconds / day(1).seconds; }
	inline uint64_t hours() const { return seconds / hour(1).seconds; }
	inline uint64_t minutes() const { return seconds / minute(1).seconds; }
	
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
	
	inline tm to_date() const {
		tm date;
		memset(&date, 0, sizeof(tm));
		Time t = *this;
		uint64_t rest_days = 0;
		date.tm_year = year_from_days(t.days(), rest_days)-1900;
		date.tm_yday = rest_days;
// 		printf(" year = %i\n", date.tm_year);
// 		printf(" year_day = %i === %lu\n", date.tm_yday, t.days());
		date.tm_hour = t.hours();
// 		printf(" hours = %i\n", date.tm_hour);
		t %= hour(1);
		date.tm_min = t.minutes();
// 		printf(" min = %i\n", date.tm_min);
		t %= minute(1);
		date.tm_sec = t.seconds;
		date.tm_mon = month(rest_days, date.tm_mday, date.tm_year+1900)-1;
		date.tm_mday += 1;
// 		printf(" sec = %i\n", date.tm_sec);
		mktime(&date);
		return date;
	}
	
	inline std::string to_string() const {
		tm date = to_date();
		char buf[64];
		sprintf(buf, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", date.tm_year+1900, date.tm_mon+1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
		return std::string(buf);
	}
	
	uint64_t seconds;
};

Time get_tm(const JSON& json) {
	tm date;
	memset(&date, 0, sizeof(date));
	date.tm_year = json["year"].Integer()-1900;
	printf("tm_year = %i\n", date.tm_year);
	date.tm_mon = json["month"].Integer()-1;
	date.tm_mday = json["day"].Integer();
	date.tm_hour = json["hour"].Integer();
	date.tm_min = json["minute"].Integer();
	date.tm_sec = json["second"].Integer();
	mktime(&date);
	printf(" JSON date: %s === %s", json.Write().c_str(), Time(date).to_string().c_str());
	Time t2(date);
	tm date2 = t2.to_date();
	Time t3(date2);
	printf(" === %s", t3.to_string().c_str());
	t2 = t3;
	
	t3 = Time(t3.to_date());
	printf(" === %s", t3.to_string().c_str());
	
	t3 = Time(t3.to_date());
	printf(" === %s", t3.to_string().c_str());
	
	t3 = Time(t3.to_date());
	printf(" === %s\n", t3.to_string().c_str());
	return Time(date);
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
	tm date = t.to_date();
	return date.tm_wday==0 || date.tm_wday==6;
}

