#include <chrono>
#include <filesystem>
#include <iostream>

int main() {
    std::filesystem::path filePath("./main");

    if (std::filesystem::exists(filePath)) {
        std::filesystem::file_time_type ftime =
            std::filesystem::last_write_time(filePath);
        std::cout << "File write time is " << ftime);
    } else {
        std::cout << "File does not exist" << std::endl;
    }

    return 0;
}