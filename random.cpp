
#pragma once

#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>

#include "JSON/include/JSON.hpp"
#include "time.cpp"

struct q_w {
	uint16_t w[4];
};

int64_t random(int64_t min, int64_t max) {
	union {
		q_w w{{0,0,0,0}};
		uint64_t qw;
	};
	for(int i=0; i<4; ++i)
		w.w[i] = rand();
	if(min == max && min == 0)
		return qw;
	return (int64_t)(qw%(uint64_t)(max+1-min)) + min;
}

uint64_t random(uint64_t modulus) {
	if(modulus == 0)
		return 0;
	return (uint64_t)random(0,0) % modulus;
}

std::string random_uuid() {
	char s[17];
	uint64_t v = random(0, 0);
	const static char c[] = "0123456789ABCDEF";
	for(uint64_t i=0; i<16; ++i)
		s[i] = c[(v>>(i<<2))&0xF];
	s[16] = 0;
	return std::string(s);
}

uint64_t generate_normal_min_max(uint64_t min, uint64_t max) {
	const uint64_t diff = max-min;
	const uint64_t count = 13;
	const uint64_t r1 = diff / count;
	uint64_t sum = random(diff - r1*count);
	for(uint64_t i=0; i<count; ++i) {
		sum += random(0, r1-1);
	}
	return sum+min;
}

uint64_t generate_normal_center_max_diff(uint64_t center, uint64_t diff) {
	return generate_normal_min_max(center-diff, center+diff);
}

void generate_random_enter_exit(Time& enter, Time& exit,
		const Time planned_start, const Time start, const Time end) {
	Time len = end-start;
	for(int i=0;; ++i) {
		enter = generate_normal_min_max(planned_start, len/5);
		if(enter >= start)
			break;
		if(i == 6) {
			enter = generate_normal_min_max(start, len/5);
			break;
		}
	}
	for(int i=0;; ++i) {
		enter = generate_normal_center_max_diff(end, len/4);
		if(enter >= start)
			break;
		if(i == 3) {
			exit = generate_normal_min_max(end - len/6, end);
			break;
		}
	}
	if(enter > exit)
		std::swap(enter, exit);
	if(enter < start)
		enter = start;
	if(exit > end)
		exit = end;
}


