
#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <map>
#include <unordered_map>

#include "JSON/src/JSON.cpp"

#include "random.cpp"
#include "time.cpp"
#include "db.cpp"
#include "name_generator.cpp"
#include "generate_snapshot.cpp"

void read_config() {
	std::fstream file("config.json");
	config.json.Clear();
	config.json.Read(file);
	config.date.start1 = get_tm(config["date"]["start1"]);
	config.date.end1 = get_tm(config["date"]["end1"]);
	config.date.start2 = get_tm(config["date"]["start2"]);
	config.date.end2 = get_tm(config["date"]["end2"]);
}

void generate_changes() {
	const uint64_t user_changes = ::config["tables"]["user"]["changes"];
	const uint64_t course_changes = ::config["tables"]["course"]["changes"];
	const uint64_t problem_changes = ::config["tables"]["problem"]["changes"];
	const uint64_t zgloszenia_problemow_changes =
		::config["tables"]["zgloszenia_problemow"]["changes"];
	
	std::unordered_set<db::user*> users;
	std::unordered_set<db::course*> courses;
	std::unordered_set<db::problem*> problems;
	std::unordered_set<db::zgloszenia_problemow*> zgloszenia;
	
	for(uint64_t i=0; i<user_changes; ++i) {
		auto e = db::get_unique(db::user::entities_list, users);
		users.insert(e);
		const std::string prev = e->name;
		while(prev == e->name)
			e->name = random_user_name();
		e->role = random_user_role();
	}
	
	for(uint64_t i=0; i<course_changes; ++i) {
		auto e = db::get_unique(db::course::entities_list, courses);
		courses.insert(e);
		const std::string prev = e->name;
		while(prev == e->name)
			e->name = random_course_name();
	}
	
	for(uint64_t i=0; i<problem_changes; ++i) {
		auto e = db::get_unique(db::problem::entities_list, problems);
		problems.insert(e);
		const std::string prev = e->culprit;
		while(prev == e->culprit)
			e->culprit = random_problem_culprit();
	}
	
	for(uint64_t i=0; i<zgloszenia_problemow_changes; ++i) {
		auto e = db::get_unique(db::zgloszenia_problemow::entities_list, zgloszenia);
		zgloszenia.insert(e);
		e->opis = "Zmodyfikowany opis!";
	}
}

int main() {
	srand(time(NULL));
	read_config();
	
	::config.json["output_head"] = "snapshots/1/";
	generate_snapshot(config.date.start1, config.date.end1);
	
	generate_changes();
	for(const auto& table : ::config.json["tables_v2"].GetObject()) {
		for(const auto& value : table.second.GetObject()) {
			::config.json["tables"][table.first][value.first] = value.second;
		}
	}
	
	::config.json["output_head"] = "snapshots/2/";
	generate_snapshot(config.date.start2, config.date.end2);
	
	printf(" work done!");
	
	return 0;
}

