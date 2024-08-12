#include "environment.hpp"
#include <iostream>

#ifdef _WIN32
#include <cstdlib> // Include for _dupenv_s
#endif

const std::string get_home_dir() {
    const char* home_dir = nullptr;

#ifdef _WIN32
    // Windows: Use _dupenv_s to safely retrieve the HOME environment variable
    size_t len = 0;
    if (_dupenv_s(&home_dir, &len, "HOME") == 0 && home_dir != nullptr) {
        std::string result(home_dir);
        free(home_dir); // Release memory allocated by _dupenv_s
        return result;
    }
#else
    // Linux and other platforms: Use the HOME environment variable
    home_dir = std::getenv("HOME");
#endif

    if (home_dir == nullptr) {
        std::cerr << "Error: Home directory not found." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return home_dir;
}
