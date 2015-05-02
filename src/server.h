/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: 
 *  Filename: server.h 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  2, 2015
 *  Time: 09:02:12
 *  Description: 
 *****************************************************************************/
#ifndef SERVER_H_
#define SERVER_H_

#include <unordered_map>
#include "socket.h"

class Server {
public:
    Server(const std::vector<UdpSocketAddr>& client_addrs): 
        _send_count(0), _port(0), _num_waiting_message(0), _timeout(0), _packet_size(0) {
        _sockfd = udp_socket();
        if (_sockfd == -1) 
            throw std::runtime_error(std::string("Create socket failed. "));
        for (const auto& i: client_addrs) 
            _clients_stats.emplace(i, Stats());
    }

    ~Server() {
        udp_close(_sockfd);
    }

    void measure() {
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

    void setPort(const uint16_t port) { _port = port; }
    void setSendCount(const int count) {_send_count = count; }
    void setPacketSize(const int size) { _packet_size = std::max(size - 28, 0); }
    // time in seconds
    void setTimeout(const int time) { 
        _timeout = time; 
        setTimeoutOnReceive(_sockfd, time);
    }
    
    uint16_t port() const { return _port; }
    int sendCount() const { return _send_count; }
    int timeout() const { return _timeout; }
    int packetSize() const { return _packet_size; }

private:
    // statistics of a certain client
    struct Stats {
        // number of times already sent
        int send_count;
        // number of times already received echo
        int reach_count;
        // total of round-trip time that received echo
        uint64_t total_time;
        // time of last packet sent
        uint64_t last_send_time;
        // content of message
        std::string message;

        Stats(): send_count(0), reach_count(0), total_time(0), last_send_time(0) { }
    };
    // socket used for sending packets
    socket_t _sockfd;
    // port used for sending packets
    uint16_t _port;
    // num of packets sending to each client
    int _send_count;
    // num of messages waiting for arrival
    int _num_waiting_message;
    // timeout time, in seconds, 0 means wait forever
    int _timeout;
    // packet size, in bytes, >= 28(udp header size and ip header size)
    int _packet_size;

    std::unordered_map<UdpSocketAddr, Stats, UdpSocketAddr::SockAddrHasher> _clients_stats;

    void send(std::unordered_map<UdpSocketAddr, Stats, UdpSocketAddr::SockAddrHasher>::iterator client_stat) {
        const UdpSocketAddr& client_addr = client_stat->first;
        client_stat->second.message = randMessage(_packet_size);
        // send error will be dealed with client unreachable
        int rtv = udp_send(_sockfd, client_stat->second.message, client_addr);
        if (rtv != -1) {
            ++_num_waiting_message;
            client_stat->second.last_send_time = gettime();
        }
        ++client_stat->second.send_count;
    }

    void recv() {
        UdpSocketAddr client_addr;
        std::string message;
        int rtv = udp_recv(_sockfd, message, client_addr);
            
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


        // message is right
        ++client_stat->second.reach_count;
        client_stat->second.total_time += gettime() - client_stat->second.last_send_time;
        client_stat->second.last_send_time = 0;

        // send another message to this client
        if(client_stat->second.send_count < _send_count) 
            send(client_stat);
    }

    void timeout() {
        uint64_t now = gettime();
        for (auto ite = _clients_stats.begin(); ite != _clients_stats.end(); ++ite) 
            if (ite->second.last_send_time && 
                now - ite->second.last_send_time > uint64_t(_timeout) * 1000000) {
                --_num_waiting_message;
                ite->second.last_send_time = 0;
                if (ite->second.send_count < _send_count)
                    send(ite);
            }
    }

    static std::string randMessage(const size_t len) {
        std::string str;
        str.reserve(len);
        for (size_t i = 0; i < len; ++i)
            str += rand() % 255 + 1;
        return std::move(str);
    }
};

#endif /* SERVER_H_ */
