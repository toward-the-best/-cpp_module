#include <boost/filesystem.hpp>
#include <iostream>

int main() {
    boost::filesystem::path dir("test_directory");

    if (boost::filesystem::create_directory(dir)) {
        std::cout << "Successfully created directory: " << dir << std::endl;
    } else {
        std::cout << "Failed to create directory: " << dir << std::endl;
    }

    return 0;
}