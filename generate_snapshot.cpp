
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
#include "time.cpp"
#include "db.cpp"

void save_to_files() {
	db::user::table_to_csv();
	db::course::table_to_csv();
	db::webinar::table_to_csv();
	db::stay::table_to_csv();
	db::problem::table_to_csv();
}

void generate_users(size_t students, size_t instructors, size_t admins) {
	new db::user();
}

void distribute_webinars(db::webinar::table_type& table) {

}

void generate_snapshot(time_t start, time_t end) {
	
	
	
	
	
	
	
	
	save_to_files();
}

