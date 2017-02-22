#include "acceptatron.h"
#include "getopt.h"
#include <cstring>
#include <functional>
#include <string>

std::function<bool(int, int)> parse_operator(const char* op)
{
	if (std::strcmp(op, "<") == 0) {
		return std::less<int>();
	}
	if (std::strcmp(op, "<=") == 0) {
		return std::less_equal<int>();
	}
	if (std::strcmp(op, "==") == 0) {
		return std::equal_to<int>();
	}
	if (std::strcmp(op, "!=") == 0) {
		return std::not_equal_to<int>();
	}
	if (std::strcmp(op, ">") == 0) {
		return std::greater<int>();
	}
	if (std::strcmp(op, ">=") == 0) {
		return std::greater_equal<int>();
	}
	return [](int, int) { return false; };
}

int main(int argc, char* argv[]) {
	int i = 0, j = 0;
	const char* message = "";
	std::function<bool(int, int)> op = [](int, int) { return false; };
	int c;
	extern char *optarg;
	while ((c = getopt(argc, argv, "a:b:m:o:")) != -1) {
		switch (c) {
			case 'a':
				i = std::stoi(optarg);
				break;
			case 'b':
				j = std::stoi(optarg);
				break;
			case 'm':
				message = optarg;
				break;
			case 'o':
				op = parse_operator(optarg);
				break;
			default:
				acceptatron::BAIL_OUT("invalid command line");
		}
	}
	
	acceptatron::cmp_ok(i, j, message, op);
	
	acceptatron::done_testing();
	return 0;
}
