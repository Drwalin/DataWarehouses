
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
	config.date.start = get_tm(config["date"]["start"]);
	config.date.end1 = get_tm(config["date"]["end1"]);
	config.date.end2 = get_tm(config["date"]["end2"]);
}







int main() {
	srand(time(NULL));
	read_config();
	
	
	
	
	return 0;
}

