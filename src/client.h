/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: client.h 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  1, 2015
 *  Time: 21:41:15
 *  Description: client
 *****************************************************************************/
#ifndef CLIENT_H_
#define CLIENT_H_

#include "socket.h"


class Client {
public:
    Client(const uint16_t port): _port(port), _sockfd(0) {
        _sockfd = udp_socket();     
        if (_sockfd == -1) 
            throw std::runtime_error(std::string("Create socket failed. "));
        _server_addr.setAddr(UdpSocketAddr::ADDR_ANY);
        _server_addr.setPort(_port);
    }

    ~Client() {
        udp_close(_sockfd);
    }

    void listen() {
        int rtv = udp_bind(_sockfd, _server_addr); 
        if (rtv == -1)
            throw std::runtime_error(std::string("Error when binding. "));
        
        std::string message;

        // TODO: how can I close the socket if this process is killed when it's running in this loop?
        while (true) {
            if (udp_recv(_sockfd, message, _server_addr) == -1)
                throw std::runtime_error(std::string("Error when receiving message. "));
            std::cout << "Client received " << message.length() + PACKET_HEADER_SIZE << " bytes message from " << _server_addr << std::endl;
            echo(message);
        }
    }

private:
    uint16_t _port;
    socket_t _sockfd;
    UdpSocketAddr _server_addr;

    void echo(const std::string& message) {
        if (udp_send(_sockfd, message, _server_addr) == -1)
            throw std::runtime_error(std::string("Error when sending echo. "));
    }

};

#endif /* CLIENT_H_ */
