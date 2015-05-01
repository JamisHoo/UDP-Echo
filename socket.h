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
 *  Description: package C-style socket functions
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

struct UdpSocketAddr {
    static const std::array<uint8_t, 4> ADDR_ANY;

    sockaddr_in v4_addr;

    UdpSocketAddr(const std::string& addr_a, const uint16_t port = 0) {
        v4_addr.sin_family = AF_INET;
        setAddr(addr_a); 
        setPort(port);
    }

    UdpSocketAddr(const std::array<uint8_t, 4> addr = ADDR_ANY, const uint16_t port = 0) { 
        v4_addr.sin_family = AF_INET;
        setAddr(addr);
        setPort(port);
    }

    void setAddr(const std::string& addr_a) {
        std::array<uint8_t, 4> addr;
        int rtv = sscanf(addr_a.c_str(), "%hu.%hu.%hu.%hu", &addr[0], &addr[1], &addr[2], &addr[3]);
        if (rtv != 4) throw std::invalid_argument(addr_a);
        setAddr(addr); 
    }

    void setAddr(const std::array<uint8_t, 4> addr) {
        uint8_t* v4_addr_addr = (uint8_t*)(&v4_addr.sin_addr.s_addr);
        v4_addr_addr[0] = addr[0];
        v4_addr_addr[1] = addr[1];
        v4_addr_addr[2] = addr[2];
        v4_addr_addr[3] = addr[3];
    }
    
    void setPort(const uint16_t port) { v4_addr.sin_port = htons(port); }
    uint16_t port() const { return ntohs(v4_addr.sin_port); }
    size_t length() const { return sizeof(v4_addr); }
    const sockaddr_in* get() const { return &v4_addr; }
    sockaddr_in* get() { return &v4_addr; }
};

// output ip:port
std::ostream& operator<<(std::ostream& out, UdpSocketAddr& addr);


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
int udp_recv(const socket_t sockfd, std::string& str, UdpSocketAddr& desc_addr);


// returns 0 on sucess
// -1 on error
int udp_close(socket_t sockfd);
    


#endif /* SOCKET_H_ */
