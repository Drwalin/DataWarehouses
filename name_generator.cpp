
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

#include "random.cpp"
#include "db.cpp"

std::string random_user_name() {
	static JSON first_names, second_names, third_names;
	static int static_execution = 1;
	if(static_execution) {
		static_execution = 0;
		std::ifstream a(config["data"]["names"]["first"].String());
		std::ifstream b(config["data"]["names"]["second"].String());
		std::ifstream c(config["data"]["names"]["third"].String());
		first_names.Read(a);
		second_names.Read(b);
		third_names.Read(c);
	}
	
	if(first_names.size() && second_names.size() && third_names.size()) {
	} else {
		printf(" No names available");
		return random_uuid() + " " + random_uuid();
	}
	
	std::string ret = first_names[random(0,0)%first_names.size()].String() + " "
// 		+ second_names[random(0,0)%second_names.size()].String() + " "
		+ third_names[random(0,0)%third_names.size()].String();
// 	printf(" name = '%s'\n", ret.c_str());
	return ret;
}

std::string random_user_role() {
	const static std::vector<std::string> roles = {
		"student",
		"admin",
		"prowadzacy"
	};
	return roles[random(0,0)%roles.size()];
}

std::string random_problem_type() {
	const static std::vector<std::string> types = {
		"problem z łączem internetowym serwera",
		"problem z łączem internetowym użytkownika",
		"problem z łączem internetowym miejskiej",
		"awaria sieci elektrycznej miejskiej",
		"przeciążenie",
		"awaria"
	};
	return types[random(0,0)%types.size()];
}

std::string random_problem_culprit() {
	const static std::vector<std::string> culprits = {
		"student",
		"serwer",
		"nezarejestrowano",
		"prowadzacy"
	};
	return culprits[random(0,0)%culprits.size()];
}

std::string random_course_name() {
	static JSON a, b;
	static int static_execution = true;
	if(static_execution) {
		static_execution = false;
		std::ifstream s1(config["data"]["languages"].String());
		std::ifstream s2(config["data"]["libraries"].String());
		a.Read(s1);
		b.Read(s2);
	}
	
	if(a.size() && b.size()) {
	} else {
		printf(" No libralanguages and libraries available");
		return random_uuid() + " " + random_uuid();
	}
	
	return a[random(0,0)%a.size()].String() + " "
		+ b[random(0,0)%b.size()].String();
}

