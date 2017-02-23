#include "acceptatron.h"
#include "getopt.h"
#include <string>

using namespace acceptatron;

int main(int argc, char* argv[])
{
	size_t tests = 0, failures = 0;
	bool print_plan = false;
	int c;
	extern char *optarg;
	while ((c = getopt(argc, argv, "t:f:p")) != -1) {
		switch (c) {
			case 't':
				tests = std::stoul(optarg);
				break;
			case 'f':
				failures = std::stoul(optarg);
				break;
			case 'p':
				print_plan = true;
				break;
			default:
				BAIL_OUT("invalid command line");
		}
	}
	if (print_plan) {
		plan(tests);
	}
	for (size_t i = 0; i < tests; ++i) {
		if (failures) {
			--failures;
			fail(APPEND_LOC(std::to_string(i)));
			continue;
		}
		pass(APPEND_LOC(std::to_string(i)));
	}
	done_testing(tests);
	return 0;
}
