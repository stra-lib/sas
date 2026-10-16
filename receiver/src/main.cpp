#include <print>

#include "receiver.hpp"


int main(int argc, char* argv[])
{

    std::println(PROJECT_NAME " " PROJECT_VERSION "\n");


    if (argc < 2)
    {
        std::println("error: invalid argument count");
        std::println("usage: SasReceiver <service>\n", argc);
        return 1;
    }


    Receiver app = {};
    app.run(argv[1]);

	return 0;
}
