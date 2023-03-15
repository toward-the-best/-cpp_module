#include <arpa/inet.h>
#include <cstring>
#include <iostream>

std::string getNetwork(const std::string ip_addr_str,
                       const std::string netmask_str) {
    try {
        // Convert the IP address and netmask from string format to binary
        // format
        in_addr_t ip_addr = inet_addr(ip_addr_str.c_str());
        in_addr_t netmask = inet_addr(netmask_str.c_str());

        // Perform a bitwise AND operation between the IP address and the
        // netmask
        in_addr_t network_addr = ip_addr & netmask;

        // Convert the network address from binary format to string format
        char network_addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &network_addr, network_addr_str, INET_ADDRSTRLEN);
        return network_addr_str;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
    return "0.0.0.0";
}

int main() {
    const std::string ip_addr_str = "192.168.1.100";
    const std::string netmask_str = "255.255.255.0";
    const std::string network_str = "192.168.1.0";
    std::string network_addr_str = getNetwork(ip_addr_str, netmask_str);
    if (!network_addr_str.compare(network_str.c_str())) {
        std::cout << "Match Network address: " << network_addr_str << std::endl;
    } else {
        std::cout << "Miss Match Network address: " << network_addr_str
                  << std::endl;
    }

    return 0;
}
