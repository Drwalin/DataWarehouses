
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

#include "random.cpp"
#include "time.cpp"
#include "db.cpp"

void save_to_files() {
	db::user::table_to_csv();
	db::course::table_to_csv();
	db::webinar::table_to_csv();
	db::stay::table_to_csv();
	db::problem::table_to_csv();
}

std::vector<db::user*> generate_users(uint64_t students, uint64_t instructors,
		uint64_t admins) {
	std::vector<db::user*> ret;
	for(uint64_t i=0; i<students; ++i) {
		db::user* u = new db::user();
		u->role = "student";
		u->add();
		ret.emplace_back(u);
	}
	for(uint64_t i=0; i<instructors; ++i) {
		db::user* u = new db::user();
		u->role = "prowadzacy";
		u->add();
		ret.emplace_back(u);
	}
	for(uint64_t i=0; i<admins; ++i) {
		db::user* u = new db::user();
		u->role = "admin";
		u->add();
		ret.emplace_back(u);
	}
	return ret;
}

std::vector<db::course*> generate_courses(uint64_t courses) {
	std::vector<db::course*> ret;
	for(uint64_t i=0; i<courses; ++i) {
		db::course* c = new db::course();
		c->add();
		ret.emplace_back(c);
	}
	return ret;
}



void distribute_one_day_webinars(const std::vector<db::webinar*>& webinars,
		Time today) {
	today = today.rounded_down_to_full_days();
	std::vector<Time> hours;
	for(int i=7; i<20; ++i)
		hours.emplace_back(today + hour(i));
	Time max_end = hours.back() + hour(1);
	for(db::webinar* w : webinars) {
		w->planned_start = hours[random(0, hours.size()-1)];
		w->planned_end = w->planned_start + hour(random(1, 2)+random(0, 1));
		if(w->planned_end > max_end)
			w->planned_end = max_end;
		w->add();
	}
}

std::vector<db::webinar*> distribute_webinars(
		std::vector<std::vector<db::webinar*>>& day_webinars,
		uint64_t count, Time start, Time end) {
	start = start.rounded_down_to_full_days();
	end = end.rounded_down_to_full_days();
	uint64_t all_days = (end-start)/day(1);
	
	std::vector<db::webinar*> webinars;
	day_webinars.resize(all_days);
	
	for(uint64_t i=0; i<count; ++i) {
		uint64_t select_day;
		for(;;) {
			select_day = random(0, all_days-1);
			if(random(0, 100)==0)
				break;
			if(is_weekend(start + day(select_day)))
				break;
		}
		db::webinar* w = new db::webinar;
		day_webinars[select_day].emplace_back(w);
		webinars.emplace_back(w);
		w->course =
			db::course::entities_list[
				random(0, db::course::entities_list.size()-1)
			];
	}
	
	for(uint64_t i=0; i<day_webinars.size(); ++i) {
		distribute_one_day_webinars(day_webinars[i], start + day(i));
	}
	return webinars;
}

void distribute_stays_over_one_day(std::vector<db::stay*>& new_stays,
		const std::vector<db::webinar*>& webinars) {
	
}

void generate_snapshot(Time start, Time end) {
	std::vector<std::vector<db::webinar*>> day_webinars;
	auto new_users = generate_users(config["tables"]["user"]["size"],
			config["tables"]["user"]["number_prowadzacy"],
			config["tables"]["user"]["number_admins"]);
	auto new_courses = generate_courses(config["tables"]["course"]["size"]);
	auto webinars = distribute_webinars(day_webinars,
			config["tables"]["webinar"]["size"], start, end);
	std::vector<std::vector<db::stay*>> stays;
	stays.resize(day_webinars.size());
	for(uint64_t i=0; i<day_webinars.size(); ++i) {

	}
	
	(new db::stay())->add();
	(new db::problem())->add();
	
	
	
	
	
	save_to_files();
}

