#include "acceptatron.h"

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		acceptatron::diag(argv[i]);
	}
	return 0;
}
