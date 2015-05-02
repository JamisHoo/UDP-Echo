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
 *  Description: package of C-style socket functions and system functions
 *****************************************************************************/
#include "socket.h"
#include <sys/time.h>


const std::array<uint8_t, 4> UdpSocketAddr::ADDR_ANY({ 0, 0, 0, 0 });

std::array<uint8_t, 4> text2ip(const std::string& addr_text) {
    std::array<uint8_t, 4> addr_ip;
    int rtv = sscanf(addr_text.c_str(), "%hhu.%hhu.%hhu.%hhu", &addr_ip[0], &addr_ip[1], &addr_ip[2], &addr_ip[3]);
    if (rtv != 4) throw std::invalid_argument(addr_text);
    return addr_ip;
}

std::ostream& operator<<(std::ostream& out, const UdpSocketAddr& addr) {
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
    constexpr size_t buffer_size = 1 << 16;
    char buffer[buffer_size];

    socklen_t socklen = dest_addr.length();


    int return_value = ::recvfrom(sockfd, buffer, buffer_size - 1, 0, (sockaddr*)dest_addr.get(), &socklen);

    if (return_value == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return -2;

    if (return_value != -1)
        buffer[return_value] = 0;
    buffer[buffer_size - 1] = 0;
    str.assign(buffer);

    return return_value;
}

int setTimeoutOnReceive(const socket_t sockfd, const size_t timeout) {
    timeval tv;
    tv.tv_sec = timeout;
    // wait for extra 500 us
    tv.tv_usec = 500;
    return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));
}

int udp_close(const socket_t sockfd) {
    return ::close(sockfd);
}


uint64_t gettime() {
    timeval time;
    gettimeofday(&time, 0);
    return time.tv_sec * 1000000 + time.tv_usec;
}


