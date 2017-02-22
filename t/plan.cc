#include "acceptatron.h"
#include "getopt.h"
#include <string>

int main(int argc, char* argv[])
{
	size_t tests = 0;
	int c;
	extern char *optarg;
	while ((c = getopt(argc, argv, "p:")) != -1) {
		switch (c) {
			case 'p':
				tests = std::stoul(optarg);
				break;
			default:
				acceptatron::BAIL_OUT("invalid command line");
		}
	}
	acceptatron::plan(tests);
	return 0;
}
