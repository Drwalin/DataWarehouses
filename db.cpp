
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

#include "time.cpp"

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

namespace db {
	template<typename T, typename ..._args>
	std::string generate_unique(const std::unordered_map<std::string, T>& table,
			std::string(*random)(_args...), _args... args) {
		while(true) {
			std::string uuid = random(args...);
			if(table.contains(uuid))
				return uuid;
		}
	}
	
	template<typename T>
	class table : public std::enable_shared_from_this<T> {
	public:
		table() = default;
		virtual ~table() = default;
		std::string id;
		static std::unordered_map<std::string, std::shared_ptr<T>>& entities() {
			static std::unordered_map<std::string, std::shared_ptr<T>> ent;
			return ent;
		}
		virtual void add() {
			entities()[id] = this->shared_from_this();
		}
		virtual void to_csv(std::ostream& out) {
			out << id;
		}
		virtual std::string _tab()=0;
		static std::string table_name() {
			static T* temp = new T;
			return temp->_tab();
		}
		static void table_to_csv() {
			std::string table_name = table::entities().begin()->second->_tab();
			std::string file_name = ::config["tables"][table_name]["csv"];
			std::ofstream file(file_name);
			for(auto it : entities()) {
				it.second->to_csv(file);
				file << "\n";
			}
		}
		static const JSON& config() {
			return ::config["tables"][table_name()];
		}
	};
	
	class user : public table<user> {
	public:
		std::string name;
		std::string role;
		virtual void to_csv(std::ostream& out) override {
			out << "," << name;
			out << "," << role;
		}
		virtual std::string _tab() override { return "user"; }
	};
	
	class course : public table<course> {
	public:
		std::string name;
		int ects;
		int semester;
		
		static std::unordered_map<std::string, std::shared_ptr<course>> unique_name;
		virtual void add() override {
			table<course>::add();
			unique_name[name] = this->shared_from_this();
		}
		virtual void to_csv(std::ostream& out) override {
			out << "," << name;
			out << "," << ects;
			out << "," << semester;
		}
		virtual std::string _tab() override { return "course"; }
	};
	
	class webinar : public table<webinar> {
	public:
		class course* course;
		time_t planned_start;
		time_t planned_end;
		time_t start;
		time_t end;
		bool has_problems;
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
		time_t entry_date;
		time_t exit_date;
		class webinar* webinar;
		class user* user;
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
		class webinar* webinar;
		int time_minutes;
		std::string type;
		std::string culprit;
		virtual void to_csv(std::ostream& out) override {
			out << "," << webinar->id;
			out << "," << type;
			out << "," << culprit;
		}
		virtual void add() override {
			table<problem>::add();
			webinar->has_problems = true;
		}
		virtual std::string _tab() override { return "problem"; }
	};
}

