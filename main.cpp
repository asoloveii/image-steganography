#include <fmt/core.h>
#include "core/core.h"

auto main(int argc, char *argv[]) -> int {
    switch (argc) {
        case 4:
            execute(argv[1], argv[2], argv[3]);
            break;
        case 3:
            execute(argv[1], argv[2]);
            break;
        case 2:
            execute(argv[1]);
            break;
        case 1:
            execute();
            break;
        default:
            throw std::invalid_argument("Invalid number of arguments, try -help, -h to know more about the program.");
    }

}
