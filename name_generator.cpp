
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

std::string generate_user_name() {
	static JSON first_names, second_names, third_names;
	static int static_execution = true;
	if(static_execution) {
		static_execution = false;
		first_names.Read(config["data"]["names"]["first"]);
		second_names.Read(config["data"]["names"]["second"]);
		third_names.Read(config["data"]["names"]["third"]);
	}
	
	if(first_names.size() && second_names.size() && third_names.size()) {
	} else {
		printf(" No names available");
		return random_uuid() + " " + random_uuid();
	}
	
	return first_names[random(0,0)%first_names.size()].String() + " "
// 		+ second_names[random(0,0)%second_names.size()].String() + " "
		+ third_names[random(0,0)%third_names.size()].String();
}

std::string random_user_role() {
	const static std::vector<std::string> roles = {
		"student",
		"admin",
		"prowadzący"
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
		"prowadzący"
	};
	return culprits[random(0,0)%culprits.size()];
}

std::string random_course_name() {
	static JSON a, b;
	static int static_execution = true;
	if(static_execution) {
		static_execution = false;
		a.Read(config["data"]["languages"]);
		b.Read(config["data"]["libraries"]);
	}
	
	if(a.size() && b.size()) {
	} else {
		printf(" No libralanguages and libraries available");
		return random_uuid() + " " + random_uuid();
	}
	
	return a[random(0,0)%a.size()].String() + " "
		+ b[random(0,0)%b.size()].String();
}

