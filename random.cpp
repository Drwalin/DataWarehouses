
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

struct q_w {
	uint16_t w[4];
};

int64_t random(int64_t min=0, int64_t max=0) {
	union {
		q_w w{{0,0,0,0}};
		uint64_t qw;
	};
	for(int i=0; i<4; ++i)
		w.w[i] = rand();
	if(min == max)
		return qw;
	max++;
	return (int64_t)(qw%(uint64_t)(max-min)) + min;
}

std::string random_uuid() {
	char s[17];
	uint64_t v = random(0, 0);
	const static char c[] = "0123456789ABCDEF";
	for(uint64_t i=0; i<16; ++i)
		s[i] = c[(v>>(i<<2))&0xF];
	s[16] = 0;
	return std::string(s+2);
}

