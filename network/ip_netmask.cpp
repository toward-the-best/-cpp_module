#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>

bool check_ipv6_network(const char *ipv6_address_str,
                        const char *network_address_str, int netmask_len) {
    // Create a socket address structure for the IPv6 address
    struct sockaddr_in6 ipv6_address;
    std::memset(&ipv6_address, 0, sizeof(ipv6_address));
    ipv6_address.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, ipv6_address_str, &ipv6_address.sin6_addr) != 1) {
        std::cerr << "Invalid IPv6 address: " << ipv6_address_str << std::endl;
        return false;
    }

    // Create a socket address structure for the network address
    struct sockaddr_in6 network_address;
    std::memset(&network_address, 0, sizeof(network_address));
    network_address.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, network_address_str, &network_address.sin6_addr) !=
        1) {
        std::cerr << "Invalid network address: " << network_address_str
                  << std::endl;
        return false;
    }

    // Apply the netmask to the network address
    for (int i = 0; i < netmask_len; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;
        network_address.sin6_addr.s6_addr[byte_index] &=
            ~(1 << (7 - bit_index));
    }

    // Compare the masked IPv6 address to the masked network address
    for (int i = 0; i < 16; i++) {
        if (ipv6_address.sin6_addr.s6_addr[i] !=
            network_address.sin6_addr.s6_addr[i]) {
            return false;
        }
    }

    return true;
}

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

    if (check_ipv6_network("2001:db8:1234:5678::1",
                           "2001:db8:1234:5678::", 64)) {
        std::cout << "IPv6 address is in network range" << std::endl;
    } else {
        std::cout << "IPv6 address is not in network range" << std::endl;
    }

    return 0;
}
