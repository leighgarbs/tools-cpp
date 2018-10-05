#include "miscNetworking.hpp"

#include <cstring>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Ipv4Address.hpp"
#include "MacAddress.hpp"

//==============================================================================
// Retrieves the MAC address corresponding to the given interface name
//==============================================================================
bool miscNetworking::getMacAddress(const std::string& interface_name,
                                   MacAddress&        mac_address)
{
#if defined LINUX
    // Getting MAC and IP addresses requires a socket, doesn't matter what kind
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock_fd == -1)
    {
        return false;
    }

    // Fill out an ifreq with name of the target interface
    ifreq iface;
    strcpy(iface.ifr_name, interface_name.c_str());

    // Request our MAC address
    int ret = ioctl(sock_fd, SIOCGIFHWADDR, &iface);

    close(sock_fd);

    if (ret == -1)
    {
        return false;
    }

    // Give the user their MAC address
    mac_address.readRaw(
        reinterpret_cast<unsigned char*>(iface.ifr_hwaddr.sa_data));

    return true;
#else
    return false;
#endif
}

//==============================================================================
// Retrieves an IPv4 address corresponding to the given interface name
//==============================================================================
bool miscNetworking::getIpv4Address(const std::string& interface_name,
                                    Ipv4Address&       ipv4_address)
{
#if defined LINUX
    // Getting MAC and IP addresses requires a socket, doesn't matter what kind
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock_fd == -1)
    {
        return false;
    }

    // Fill out an ifreq with name of the target interface
    ifreq iface;
    strcpy(iface.ifr_name, interface_name.c_str());

    // Request our IP address
    int ret = ioctl(sock_fd, SIOCGIFADDR, &iface);

    close(sock_fd);

    if (ret == -1)
    {
        return false;
    }

    // Initialize own IP address
    sockaddr_in* temp_addr = (sockaddr_in*)&iface.ifr_addr;
    ipv4_address.readRaw(
        reinterpret_cast<unsigned char*>(&temp_addr->sin_addr.s_addr));

    return true;
#else
    return false;
#endif
}

//==============================================================================
// Byteswaps the buffer of memory at "buffer" of length "len".  For example a
// "len" value of 4 would be used for swapping a single 32-bit integer
//==============================================================================
void miscNetworking::byteswap(unsigned char* buffer, unsigned int len)
{
    // The third memory location for all byteswaps
    unsigned char temp;

    // Work from both sides of "buffer" simultaneously
    for (unsigned int i = 0; i < len / 2; i++)
    {
        // A standard 3-step swap
        temp = buffer[i];
        buffer[i] = buffer[(len - i) - 1];
        buffer[(len - i) - 1] = temp;
    }
}
