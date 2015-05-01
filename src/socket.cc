/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: socket.cc 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  1, 2015
 *  Time: 14:11:30
 *  Description: package of C-style functions
 *****************************************************************************/
#include "socket.h"


const std::array<uint8_t, 4> UdpSocketAddr::ADDR_ANY({ 0, 0, 0, 0 });

std::ostream& operator<<(std::ostream& out, UdpSocketAddr& addr) {
    uint8_t* v4_addr = (uint8_t*)(&addr.v4_addr.sin_addr.s_addr);
    out << unsigned(v4_addr[0]) << '.' 
        << unsigned(v4_addr[1]) << '.'
        << unsigned(v4_addr[2]) << '.'
        << unsigned(v4_addr[3]) << ':'
        << addr.port();
    return out;
}

socket_t udp_socket() {
    return ::socket(AF_INET, SOCK_DGRAM, 0);
}

int udp_bind(const socket_t sockfd, const UdpSocketAddr& serv_addr) {
    return ::bind(sockfd, (sockaddr*)serv_addr.get(), serv_addr.length());
}


int udp_send(const socket_t sockfd, const std::string& message, const UdpSocketAddr& dest_addr) {
    return ::sendto(sockfd, message.data(), message.length(), 0, (sockaddr*)dest_addr.get(), dest_addr.length());
}

int udp_recv(const socket_t sockfd, std::string& str, UdpSocketAddr& dest_addr) {
    constexpr size_t buffer_size = 1 << 12;
    char buffer[buffer_size];

    socklen_t socklen = dest_addr.length();

    /*
    for (int i = 0; i < sizeof(dest_addr); ++i)
        std::cout << (unsigned(((char*)(&dest_addr))[i]) & 0xff) << ' ';
    std::cout << std::endl;
    std::cout << "socklen == " << socklen << std::endl;
    std::cout << ntohs(dest_addr.get()->sin_port) << std::endl;
    */

    int return_value = ::recvfrom(sockfd, buffer, buffer_size - 1, 0, (sockaddr*)dest_addr.get(), &socklen);

    /*
    for (int i = 0; i < sizeof(dest_addr); ++i)
        std::cout << (unsigned((((char*)(&dest_addr))[i])) & 0xff) << ' ';
    std::cout << std::endl;
    std::cout << "socklen == " << socklen << std::endl;
    std::cout << ntohs(dest_addr.get()->sin_port) << std::endl;
    */
    buffer[return_value] = 0;
    buffer[buffer_size - 1] = 0;
    str.assign(buffer);

    return return_value;
}

int udp_close(socket_t sockfd) {
    return ::close(sockfd);
}

