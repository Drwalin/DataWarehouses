
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
		w->planned_start = hours[random(hours.size())];
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
			select_day = random(all_days);
			if(!is_weekend(start + day(select_day)))
				break;
			if(random(100)==0)
				break;
		}
		db::webinar* w = new db::webinar;
		day_webinars[select_day].emplace_back(w);
		webinars.emplace_back(w);
		w->course =
			db::course::entities_list[
				random(db::course::entities_list.size())
			];
	}
	
	for(uint64_t i=0; i<day_webinars.size(); ++i) {
		distribute_one_day_webinars(day_webinars[i], start + day(i));
	}
	return webinars;
}

void distribute_stays_over_one_day(const std::vector<db::stay*>& new_stays,
		const std::vector<db::webinar*>& webinars) {
	if(new_stays.size() == 0 || webinars.size() == 0) {
		return;
	}
	uint64_t i=0;
	for(; i<webinars.size(); ++i) {
		auto s = new_stays[i];
		auto w = s->webinar = webinars[i];
		s->user = db::get_unique(db::user::instructors, w->users);
		Time length = w->planned_end - w->planned_start;
		s->entry_date = generate_normal_center_max_diff(
				w->planned_start, length/3);
		s->exit_date = generate_normal_center_max_diff(
				w->planned_end, length/3);
		if(s->entry_date > s->exit_date)
			std::swap(s->entry_date, s->exit_date);
		w->start = s->entry_date;
		w->end = s->exit_date;
		s->add();
	}
	for(; i<new_stays.size(); ++i) {
		auto s = new_stays[i];
		auto w = s->webinar = webinars[random(webinars.size())];
		s->user = db::get_unique(db::user::entities_list, w->users);
		generate_random_enter_exit(s->entry_date, s->exit_date,
				w->planned_start, w->start, w->end);
		s->add();
	}
}

std::vector<db::problem*> distribute_problems(
		const std::vector<db::webinar*>& webinars, uint64_t count) {
	std::vector<db::problem*> problems;
	problems.reserve(count);
	
	for(uint64_t i=0; i<count; ++i) {
		db::problem* p = new db::problem;
		p->webinar = webinars[random(webinars.size())];
		p->add();
		problems.emplace_back(p);
	}
	
	return problems;
}

void generate_snapshot(Time start, Time end) {
	
	auto new_users = generate_users(config["tables"]["user"]["size"],
			config["tables"]["user"]["number_prowadzacy"],
			config["tables"]["user"]["number_admins"]);
	
	auto new_courses = generate_courses(config["tables"]["course"]["size"]);
	
	std::vector<std::vector<db::webinar*>> day_webinars;
	auto webinars = distribute_webinars(day_webinars,
			config["tables"]["webinar"]["size"], start, end);
	
	std::vector<std::vector<db::stay*>> stays;
	stays.resize(day_webinars.size(), {});
	uint64_t stays_count = config["tables"]["stay"]["size"];
	for(uint64_t i=0; i<day_webinars.size(); ++i) {
		if(day_webinars.size()) {
			for(uint64_t j=0; j<day_webinars[i].size()*2; ++j) {
				stays[i].emplace_back(new db::stay);
				if(stays_count)
					--stays_count;
			}
		}
	}
	for(; stays_count;) {
		for(uint64_t i=0;; ++i) {
			uint64_t r = random(stays.size());
			if(day_webinars[r].size() != 0) {
				stays[r].emplace_back(new db::stay);
				break;
			}
			if(i ==10) {
				for(uint64_t i=0; i<stays.size(); ++i) {
					if(day_webinars[i].size() != 0) {
						stays[i].emplace_back(new db::stay);
						break;
					}
				}
				break;
			}
		}
		if(stays_count)
			--stays_count;
		else
			break;
	}
	
	for(uint64_t i=0; i<day_webinars.size(); ++i) {
		distribute_stays_over_one_day(stays[i], day_webinars[i]);
	}
	
	auto problems = distribute_problems(webinars,
			config["tables"]["problem"]["size"]);
	
	save_to_files();
}

