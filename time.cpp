
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

time_t to_time(tm t) {
	return mktime(&t);
}

tm to_date(time_t t) {
	return *localtime(&t);
}

time_t get_tm(const JSON& json) {
	tm date;
	memset(&date, 0, sizeof(date));
	date.tm_year = json["year"];
	date.tm_mon = json["month"];
	date.tm_mday = json["day"];
	date.tm_hour = json["hour"];
	date.tm_min = json["minute"];
	date.tm_sec = json["second"];
	return mktime(&date);
}

time_t day(int days) {
	return 3600*24*days;
}

time_t hour(int hours) {
	return 3600*hours;
}

time_t minute(int minutes) {
	return 60*minutes;
}

time_t round_down_to_full_days(time_t t) {
	return t - (t%day(1));
}

time_t round_down_to_full_hours(time_t t) {
	return t - (t%hour(1));
}

tm operator+(tm a, time_t tb) {
	time_t ta = mktime(&a);
	ta += tb;
	return *localtime(&ta);
}

tm operator+(time_t ta, tm b) {
	time_t tb = mktime(&b) + ta;
	return *localtime(&tb);
}

tm operator+(tm a, tm b) {
	time_t tb = mktime(&b);
	return a + tb;
}

std::string to_string(time_t t) {
	tm& date = *localtime(&t);
	char buf[64];
	sprintf(buf, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", date.tm_year, date.tm_mon, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
	return std::string(buf);
}

std::ostream& operator<<(std::ostream& stream, time_t t) {
	return stream << to_string(t);
}

