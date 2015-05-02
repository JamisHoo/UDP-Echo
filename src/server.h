/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: server.h 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  2, 2015
 *  Time: 09:02:12
 *  Description: server
 *****************************************************************************/
#ifndef SERVER_H_
#define SERVER_H_

#include <unordered_map>
#include <vector>
#include <limits>
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

    void measure();

    void showStat();

    void setPort(const uint16_t port) { _port = port; }
    void setSendCount(const int count) { _send_count = count; }
    void setPacketSize(const int size) { _packet_size = std::max(size - PACKET_HEADER_SIZE, 0); }
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
        // min round-trip time
        uint64_t min_time;
        // max round-trip time
        uint64_t max_time;

        // content of message
        std::string message;

        Stats(): send_count(0), reach_count(0), total_time(0), last_send_time(0),
            min_time(std::numeric_limits<uint64_t>::max()), max_time(0){ }
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
    // packet size, in bytes, >= packet header size(udp header size(8) and ip header size(20))
    int _packet_size;

    std::unordered_map<UdpSocketAddr, Stats, UdpSocketAddr::SockAddrHasher> _clients_stats;

    // send packet to a certain client
    void send(std::unordered_map<UdpSocketAddr, Stats, UdpSocketAddr::SockAddrHasher>::iterator client_stat);

    // receive a packet 
    void recv();
    
    // receive time out
    void timeout();

    // generate a message of centain length
    static std::string randMessage(const size_t len);
};

#endif /* SERVER_H_ */
