/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: parser.h 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  1, 2015
 *  Time: 21:06:39
 *  Description: 
 *****************************************************************************/
#ifndef PARSER_H_
#define PARSER_H_

#include "socket.h"
#include <boost/program_options.hpp>
  
class OptionParser {
public:
    OptionParser(): _port(0), _is_server(0), _is_client(0), _packet_count(0), _timeout(0), _packet_size(0), all_options("Usage") {
        initialize();
    }

    void initialize();

    void parse(int, char**);

    const std::vector<UdpSocketAddr>& clientAddrs() const { return _client_addrs; }
    uint16_t port() const { return _port; } 
    std::string configfile() const { return _filename; }
    bool isServer() const { return _is_server; }
    bool isClient() const { return _is_client; }
    int32_t packetCount() const { return _packet_count; }
    int32_t packetSize() const { return _packet_size; }
    int32_t timeout() const { return _timeout; }

private:
    uint16_t _port;
    std::string _filename;
    bool _is_server;
    bool _is_client;
    int32_t _packet_count;
    int32_t _timeout;
    int32_t _packet_size;

    std::vector<UdpSocketAddr> _client_addrs;

    boost::program_options::variables_map vm;
    boost::program_options::options_description all_options;   

    void loadClientAddrs();
};


#endif /* PARSER_H_ */
