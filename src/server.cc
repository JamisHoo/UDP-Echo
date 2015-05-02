/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: server.cc 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  2, 2015
 *  Time: 20:23:16
 *  Description: server
 *****************************************************************************/
#include "server.h"
#include <iostream>

void Server::measure() {
    UdpSocketAddr server_addr(UdpSocketAddr::ADDR_ANY, _port);
    if (udp_bind(_sockfd, server_addr) == -1)
        throw std::runtime_error(std::string("Error when binding. "));
    
    for (auto ite = _clients_stats.begin(); ite != _clients_stats.end(); ++ite) {
        send(ite);
    }

    while (_num_waiting_message) {
        recv();
    }
}

void Server::send(std::unordered_map<UdpSocketAddr, Stats, UdpSocketAddr::SockAddrHasher>::iterator client_stat) {
    const UdpSocketAddr& client_addr = client_stat->first;
    client_stat->second.message = randMessage(_packet_size);
    // send error will be dealed with client unreachable
    int rtv = udp_send(_sockfd, client_stat->second.message, client_addr);

    auto now = gettime();

    if (rtv != -1) {
        std::cout << "Server sent " << client_stat->second.message.length() + PACKET_HEADER_SIZE << " bytes message to " << client_stat->first 
                  << ", seq = " << client_stat->second.send_count << std::endl;
        ++_num_waiting_message;
        client_stat->second.last_send_time = now;
    }
    ++client_stat->second.send_count;
}

void Server::showStat() {
    std::cout << "\n--- statistics ---\n";
    for (const auto i: _clients_stats) {
        std::cout << "Client " << i.first << " : "
                  << "Packet transmitted/received/loss = " 
                  << i.second.send_count << '/' 
                  << i.second.reach_count << '/' 
                  << i.second.send_count - i.second.reach_count;
        if (i.second.reach_count)
            std::cout << ", "
                      << "round-trip min/avg/max = "
                      << double(i.second.min_time) / 1000 << '/' 
                      << double(i.second.total_time) / 1000 / i.second.reach_count << '/'
                      << double(i.second.max_time) / 1000 << " ms";
        std::cout << std::endl;


    }
}

void Server::recv() {
    UdpSocketAddr client_addr;
    std::string message;
    int rtv = udp_recv(_sockfd, message, client_addr);
        
    auto now = gettime();
    
    // receive timeout
    if (rtv == -2) return timeout();

    // receive error is ignored
    if (rtv == -1) return;

    auto client_stat = _clients_stats.find(client_addr);
    // if receive a message from unknown host, ignore it
    if (client_stat == _clients_stats.end()) return;
    // else receive a message from known client
    --_num_waiting_message;

    // if message is error
    if (message != client_stat->second.message) return;
    

    std::cout << "Server received " << message.length() + PACKET_HEADER_SIZE << " bytes message from " << client_stat->first
              << ", seq = " << client_stat->second.send_count - 1
              << ", time = " << double(now - client_stat->second.last_send_time) / 1000 << "ms" << std::endl;

    // message is right
    ++client_stat->second.reach_count;
    client_stat->second.total_time += now - client_stat->second.last_send_time;
    client_stat->second.max_time = std::max(client_stat->second.max_time, now - client_stat->second.last_send_time);
    client_stat->second.min_time = std::min(client_stat->second.min_time, now - client_stat->second.last_send_time);
    client_stat->second.last_send_time = 0;

    // send another message to this client
    if(client_stat->second.send_count < _send_count) 
        send(client_stat);
}

void Server::timeout() {
    uint64_t now = gettime();
    for (auto ite = _clients_stats.begin(); ite != _clients_stats.end(); ++ite) 
        if (ite->second.last_send_time && 
            now - ite->second.last_send_time > uint64_t(_timeout) * 1000000) {
            
            std::cout << "Server's waiting for message from " << ite->first << " timed out " 
                      << "seq = " << ite->second.send_count - 1
                      << " time = " << double(now - ite->second.last_send_time) / 1000 << "ms" << std::endl;

            --_num_waiting_message;
            ite->second.last_send_time = 0;
            if (ite->second.send_count < _send_count)
                send(ite);
        }
}

std::string Server::randMessage(const size_t len) {
    std::string str;
    str.reserve(len);
    for (size_t i = 0; i < len; ++i)
        str += rand() % 255 + 1;
    return std::move(str);
}



