#include <print>

#include "sender.hpp"

/*
 *
 * parse arg port and ip       #
 *
 * connect to receiver         .
 *
 * set up system audio capture .
 *
 * start capture loop          .
 *
 * start send audio loop       .
 *
 */

int main(const int argc, char* argv[])
{

	std::println(PROJECT_NAME " " PROJECT_VERSION "\n");


    if (argc < 3)
    {
        if (argc > 1) std::println("error: invalid argument count");
        std::println("usage: SasSender <host> <service>\n", argc);
        return 1;
    }


    Sender app = {};
    app.run(argv[1], argv[2]);

	return 0;
}
