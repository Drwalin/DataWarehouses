
#pragma once

#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "JSON/include/JSON.hpp"

#include "time.cpp"
#include "random.cpp"

struct {
	struct {
		time_t start;
		time_t end1;
		time_t end2;
	} date;
	const JSON& operator[](const char* key) const {
		return json[key];
	}
	JSON json;
} config;

std::string random_user_name();
std::string random_user_role();
std::string random_problem_type();
std::string random_problem_culprit();
std::string random_course_name();

namespace db {
	template<typename T, typename ..._args>
	std::string generate_unique(const std::unordered_map<std::string, T>& table,
			std::string(*random)(_args...), _args... args) {
		for(int i=0; i<100; ++i) {
			std::string uuid = random(args...);
			if(table.find(uuid) == table.end())
				return uuid;
		}
		while(true) {
			std::string uuid = random(args...) + " " + random_uuid();
			if(table.find(uuid) == table.end())
				return uuid;
		}
	}
	
	template<typename T>
	class table : public std::enable_shared_from_this<T> {
	public:
		table() = default;
		virtual ~table() = default;
		std::string id;
		using table_type = std::unordered_map<std::string, std::shared_ptr<T>>;
		using array_type = std::vector<std::shared_ptr<T>>;
		using super = table<T>;
		inline static table_type entities;
		inline static std::vector<std::shared_ptr<T>> entities_list;
		virtual void add() {
			id = generate_unique(entities, random_uuid);
			entities[id] = this->shared_from_this();
			entities_list.emplace_back(this->shared_from_this());
		}
		virtual void to_csv(std::ostream& out) {
			out << id;
		}
		virtual std::string _tab()=0;
		inline static std::string table_name() {
			static T* temp = new T;
			return temp->_tab();
		}
		inline  static void table_to_csv() {
			std::string table_name = table::entities.begin()->second->_tab();
			std::string file_name = ::config["tables"][table_name]["csv"];
			std::ofstream file(file_name);
			for(auto it : entities) {
				it.second->to_csv(file);
				file << "\n";
			}
		}
		inline static const JSON& config() {
			return ::config["tables"][table_name()];
		}
	};
	
	class user : public table<user> {
	public:
		std::string name;
		std::string role;
		std::unordered_set<class stay*> stays;
		
		inline static array_type instructors;
		inline static array_type others;
		
		virtual void to_csv(std::ostream& out) override {
			out << "," << name;
			out << "," << role;
		}
		virtual void add() override {
			super::add();
			name = random_user_name();
			if(role == "prowadzący")
				instructors.emplace_back(shared_from_this());
			else
				others.emplace_back(shared_from_this());
		}
		virtual std::string _tab() override { return "user"; }
	};
	
	class course : public table<course> {
	public:
		course() = default;
		~course() = default;
		std::string name;
		int ects;
		int semester;
		
		inline static table_type unique_name;
		
		virtual void add() override {
			super::add();
			unique_name[name] = this->shared_from_this();
		}
		virtual void to_csv(std::ostream& out) override {
			name = generate_unique(entities, random_course_name);
			ects = 0;
			for(int i=0; i<6; ++i)
				ects += random(0, 1);
			if(ects | random(0, 5))
				ects = random(1, 2);
			semester = random(1, 10);
			if(random(0, 15) == 0)
				semester = random(11, 15);
			out << "," << name;
			out << "," << ects;
			out << "," << semester;
		}
		virtual std::string _tab() override { return "course"; }
	};
	
	class webinar : public table<webinar> {
	public:
		webinar() = default;
		~webinar() = default;
		class course* course;
		time_t planned_start;
		time_t planned_end;
		time_t start;
		time_t end;
		bool has_problems;
		std::unordered_set<class stay*> stays;
		std::unordered_set<class user*> users;
		std::unordered_set<class problem*> problems;
		virtual void to_csv(std::ostream& out) override {
			out << "," << course->id;
			out << "," << to_string(planned_start);
			out << "," << to_string(planned_end);
			out << "," << to_string(start);
			out << "," << to_string(end);
			out << "," << (has_problems ? "true" : "false");
		}
		virtual std::string _tab() override { return "webinar"; }
	};
	
	class stay : public table<stay> {
	public:
		stay() = default;
		~stay() = default;
		time_t entry_date;
		time_t exit_date;
		class webinar* webinar;
		class user* user;
		virtual void add() override {
			super::add();
			webinar->stays.insert(this);
			user->stays.insert(this);
		}
		virtual void to_csv(std::ostream& out) override {
			out << "," << to_string(entry_date);
			out << "," << to_string(exit_date);
			out << "," << webinar->id;
			out << "," << user->id;
		}
		virtual std::string _tab() override { return "stay"; }
	};
	
	class problem : public table<problem> {
	public:
		problem() = default;
		~problem() = default;
		class webinar* webinar;
		int time_minutes;
		std::string type;
		std::string culprit;
		virtual void to_csv(std::ostream& out) override {
			type = random_problem_type();
			culprit = random_problem_culprit();
			out << "," << webinar->id;
			out << "," << type;
			out << "," << culprit;
		}
		virtual void add() override {
			super::add();
			webinar->has_problems = true;
			webinar->problems.insert(this);
		}
		virtual std::string _tab() override { return "problem"; }
	};
}

