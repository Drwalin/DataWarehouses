
#pragma once

#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <map>
#include <unordered_map>

uint64_t random(uint64_t min=0, uint64_t max=0) {
	using q_w = struct { uint16_t w[4]; };
	union {
		q_w w;
		uint64_t qw;
	};
	if(min == max)
		return qw;
	return (qw%(max-min)) + min;
}

std::string random_uuid() {
	char s[24];
	sprintf(s, "%16p", (void*)random(0,0));
	return std::string(s);
}

