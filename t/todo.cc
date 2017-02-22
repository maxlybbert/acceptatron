#include "acceptatron.h"
#include "getopt.h"
#include <cstring>
#include <functional>
#include <string>

std::function<bool(int, int)> parse(const char* op)
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
	std::equal_to<int> eq;
	std::function<bool(int, int)> op(eq);
	int i = 0, j = 0;
	const char* message = "";
	int c;
	extern char *optarg;
	while ((c = getopt(argc, argv, "o:a:b:m:")) != -1) {
		switch (c) {
			case 'o':
				op = parse(optarg);
				break;
			case 'a':
				i = std::stoi(optarg);
				break;
			case 'b':
				j = std::stoi(optarg);
				break;
			case 'm':
				message = optarg;
				break;
			default:
				acceptatron::BAIL_OUT("invalid command line");
		}
	}

	acceptatron::todo(1);	
	acceptatron::ok(op(i, j), message);
	acceptatron::end_todo();	
	
	acceptatron::done_testing();
	return 0;
}
