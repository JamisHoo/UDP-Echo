/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: socket.h 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  1, 2015
 *  Time: 14:09:02
 *  Description: package C-style socket functions and system functions
 *****************************************************************************/
#ifndef SOCKET_H_
#define SOCKET_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <array>
#include <stdexcept>
#include <iostream>

// convert ipv4 address from numbers-and-dots notation to array
std::array<uint8_t, 4> text2ip(const std::string& addr_text);

// ipv4 udp socket address
struct UdpSocketAddr {
    static const std::array<uint8_t, 4> ADDR_ANY;

    sockaddr_in v4_addr;

    UdpSocketAddr(const std::array<uint8_t, 4> addr = ADDR_ANY, const uint16_t port = 0) { 
        v4_addr.sin_family = AF_INET;
        setAddr(addr);
        setPort(port);
    }

    void setAddr(const std::array<uint8_t, 4> addr) {
        uint8_t* v4_addr_addr = (uint8_t*)(&v4_addr.sin_addr.s_addr);
        v4_addr_addr[0] = addr[0];
        v4_addr_addr[1] = addr[1];
        v4_addr_addr[2] = addr[2];
        v4_addr_addr[3] = addr[3];
    }

    // used for unordered_map
    bool operator==(const UdpSocketAddr& sockaddr) const {
        return addr() == sockaddr.addr() && port() == sockaddr.port();
    }

    // used for unordered_map
    struct SockAddrHasher {
        std::size_t operator()(const UdpSocketAddr& k) const {
            auto addr = k.addr();
            uint32_t addr_uint = (addr[0] << 24) & (addr[1] << 16) & (addr[2] << 8) & addr[3];
            return std::hash<uint32_t>()(addr_uint) ^
                   std::hash<uint16_t>()(k.port());
        }
    };

    std::array<uint8_t, 4> addr() const {
        uint8_t* v4_addr_addr = (uint8_t*)(&v4_addr.sin_addr.s_addr);
        return { v4_addr_addr[0], v4_addr_addr[1],
                 v4_addr_addr[2], v4_addr_addr[3] };
    }
    void setPort(const uint16_t port) { v4_addr.sin_port = htons(port); }
    uint16_t port() const { return ntohs(v4_addr.sin_port); }
    size_t length() const { return sizeof(v4_addr); }
    const sockaddr_in* get() const { return &v4_addr; }
    sockaddr_in* get() { return &v4_addr; }
};

// output ip:port
std::ostream& operator<<(std::ostream& out, const UdpSocketAddr& addr);

using socket_t = int32_t;

// returns file descriptor for the new socket on success
// -1 on error
socket_t udp_socket();

// returns 0 on success
// -1 on error
int udp_bind(const socket_t sockfd, const UdpSocketAddr& serv_addr);

// returns the number of bytes sent on success
// -1 on error
int udp_send(const socket_t sockfd, const std::string& message, const UdpSocketAddr& dest_addr);

// returns the number of bytes received on success
// -1 on error
// -2 on timeout
int udp_recv(const socket_t sockfd, std::string& str, UdpSocketAddr& desc_addr);

// sets the timeout value that recv function waits until it completes
// returns 0 on success
// -1 on error
int setTimeoutOnReceive(const socket_t sockfd, const size_t timeout);

// returns 0 on sucess
// -1 on error
int udp_close(const socket_t sockfd);

// get microsecond time
uint64_t gettime(); 


#endif /* SOCKET_H_ */
