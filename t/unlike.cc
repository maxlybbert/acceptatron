#include "acceptatron.h"
#include "getopt.h"
#include <cstring>
#include <functional>
#include <string>

std::function<bool(int)> parse_operator(const char* op)
{
	if (std::strcmp(op, "even") == 0) {
		return [](int i) { return i % 2 == 0; };
	}
	if (std::strcmp(op, "odd") == 0) {
		return [](int i) { return i % 2; };
	}
	return [](int) { return true; };
}

int main(int argc, char* argv[]) {
	int i = 0;
	const char* message = "";
	std::function<bool(int)> op = [](int) { return true; };
	int c;
	extern char *optarg;
	while ((c = getopt(argc, argv, "a:m:o:")) != -1) {
		switch (c) {
			case 'a':
				i = std::stoi(optarg);
				break;
			case 'o':
				op = parse_operator(optarg);
				break;
			case 'm':
				message = optarg;
				break;
			default:
				acceptatron::BAIL_OUT("invalid command line");
		}
	}
	
	acceptatron::unlike(i, op, message);
	
	acceptatron::done_testing();
	return 0;
}
