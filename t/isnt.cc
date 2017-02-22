#include "acceptatron.h"
#include "getopt.h"
#include <cstring>
#include <string>

int main(int argc, char* argv[]) {
	int i = 0, j = 0;
	const char* message = "";
	int c;
	extern char *optarg;
	while ((c = getopt(argc, argv, "a:b:m:")) != -1) {
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
			default:
				acceptatron::BAIL_OUT("invalid command line");
		}
	}
	
	acceptatron::isnt(i, j, message);
	
	acceptatron::done_testing();
	return 0;
}
