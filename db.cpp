
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
#include <algorithm>
#include <functional>

#include "JSON/include/JSON.hpp"

#include "time.cpp"
#include "random.cpp"

struct {
	struct {
		Time start1;
		Time end1;
		Time start2;
		Time end2;
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
	T get_unique(const std::vector<T>& table,
			const std::unordered_set<T>& unique) {
		T v;
		for(int i=0; i<100; ++i) {
			v = table[random(0, table.size()-1)];
			if(unique.find(v) == unique.end())
				return v;
		}
		return v;
	}
	
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
	class table {
	public:
		table() {
			++constructed_objects;
		}
		inline static uint64_t constructed_objects = 0;
		virtual ~table() = default;
		std::string id;
		using table_type = std::unordered_map<std::string, T*>;
		using array_type = std::vector<T*>;
		using super = table<T>;
		using outter_type = T;
		inline static table_type entities;
		inline static std::vector<T*> entities_list;
		virtual void add() {
			id = generate_unique(entities, random_uuid);
			entities[id] = (T*)this;
			entities_list.emplace_back((T*)this);
		}
		virtual void to_csv(std::ostream& out) {
			out << id;
		}
		virtual std::string _tab()=0;
		inline static std::string table_name() {
			static T* temp = new T;
			return temp->_tab();
		}
		inline static void table_to_csv() {
			std::string table_name = table::entities.begin()->second->_tab();
			std::string file_name = ::config["tables"][table_name]["csv"];
			file_name = ::config["output_head"].String() + file_name;
			std::ofstream file(file_name);
			std::vector<T*> sorted;
			sorted.reserve(entities.size());
			for(const auto& it : entities)
				sorted.emplace_back(it.second);
			std::sort(sorted.begin(), sorted.end(), T().get_sort_function());
			for(auto it : sorted) {
				it->to_csv(file);
				file << "\n";
			}
		}
		inline static const JSON& config() {
			return ::config["tables"][table_name()];
		}
		virtual std::function<bool(const T*, const T*)>
			get_sort_function() const {
			return [](const T*a, const T* b) -> bool {
				return a->id < b->id;
			};
		}
	};
	
	class user : public table<user> {
	public:
		std::string name;
		std::string role;
		
		inline static array_type instructors;
		inline static array_type others;
		
		virtual void to_csv(std::ostream& out) override {
			super::to_csv(out);
			out << "," << name;
			out << "," << role;
		}
		virtual void add() override {
			super::add();
			name = random_user_name();
			if(role == "prowadzacy")
				instructors.emplace_back(this);
			else
				others.emplace_back(this);
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
			unique_name[name] = this;
		}
		virtual void to_csv(std::ostream& out) override {
			super::to_csv(out);
			name = generate_unique(entities, random_course_name);
			ects = 0;
			for(int i=0; i<6; ++i)
				ects += random(0, 1);
			if(ects==0 && random(0, 5))
				ects = random(1, 6);
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
		Time planned_start;
		Time planned_end;
		Time start;
		Time end;
		bool has_problems;
		std::unordered_set<class user*> users;
		virtual void to_csv(std::ostream& out) override {
			super::to_csv(out);
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
		Time entry_date;
		Time exit_date;
		class webinar* webinar;
		class user* user;
		virtual void add() override {
			super::add();
			webinar->users.insert(user);
		}
		virtual void to_csv(std::ostream& out) override {
			super::to_csv(out);
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
			super::to_csv(out);
			type = random_problem_type();
			culprit = random_problem_culprit();
			time_minutes = random(1, (webinar->end-webinar->start).minutes());
			out << "," << webinar->id;
			out << "," << time_minutes;
			out << "," << type;
			out << "," << culprit;
		}
		virtual void add() override {
			super::add();
			webinar->has_problems = true;
		}
		virtual std::string _tab() override { return "problem"; }
	};
	
	
	
	
	
	
	
	
	class ankieta_kurs : public table<ankieta_kurs> {
	public:
		ankieta_kurs() {
			static uint64_t _ID = 0;
			ID = ++_ID;
		}
		uint64_t ID;
		class course* course;
		int oceny[5];
		
		virtual void to_csv(std::ostream& out) override {
			out << course->id;
			for(int o : oceny)
				out << "," << o;
		}
		virtual void add() override {
			course = course::entities_list[random(course::entities_list.size())];
			for(int& o : oceny)
				o = random(1, 5);
			super::add();
		}
		virtual std::string _tab() override { return "ocena_kursu"; }
		virtual std::function<bool(const outter_type*, const outter_type*)>
			get_sort_function() const override {
			return [](const outter_type*a, const outter_type* b) -> bool {
				return a->ID < b->ID;
			};
		}
	};
	
	class ankieta_portalu : public table<ankieta_portalu> {
	public:
		ankieta_portalu() {
			static uint64_t _ID = 0;
			ID = ++_ID;
		}
		uint64_t ID;
		int oceny[3];
		
		virtual void to_csv(std::ostream& out) override {
			for(int o : oceny)
				out << "," << o;
		}
		virtual void add() override {
			for(int& o : oceny)
				o = random(1, 5);
			super::add();
		}
		virtual std::string _tab() override { return "ocena_portalu"; }
		virtual std::function<bool(const outter_type*, const outter_type*)>
			get_sort_function() const override {
			return [](const outter_type*a, const outter_type* b) -> bool {
				return a->ID < b->ID;
			};
		}
	};
	
	class zgloszenia_problemow : public table<zgloszenia_problemow> {
	public:
		zgloszenia_problemow() {
			platform = (false);
			webinar = (NULL);
			course = (NULL);
		}
		Time date;
		std::string opis;
		class problem* webinar;
		class course* course;
		bool platform;
		
		virtual void to_csv(std::ostream& out) override {
			out << date;
			out << "," << opis;
			out << "," << (webinar ? webinar->webinar->id : "Nie");
			out << "," << (course&&!webinar ? course->id : "Nie");
			if(webinar == NULL && course == NULL)
				platform = true;
			out << "," << (platform ? "Tak" : "Nie");
		}
		virtual void add() override {
			super::add();
		}
		virtual std::string _tab() override { return "zgloszenia_problemow"; }
		virtual std::function<bool(const outter_type*, const outter_type*)>
			get_sort_function() const override {
			return [](const outter_type*a, const outter_type* b) -> bool {
				return a->date < b->date;
			};
		}
	};
}

