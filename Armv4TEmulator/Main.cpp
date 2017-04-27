#include <string>

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"


int main(int argc, char* argv[])
{
	if (argc > 1) {
		if (std::string(argv[1]) == "-t") {
			argv[1] = "";
			int result = Catch::Session().run(argc, argv);
			return (result < 0xff ? result : 0xff);
		}
	}

	return 0;
}