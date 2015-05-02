/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: parser.cc 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  1, 2015
 *  Time: 20:02:34
 *  Description: 
 *****************************************************************************/
#include "parser.h"
#include <iostream>
#include <fstream>
#include <limits>

void OptionParser::initialize() {
    using namespace std;
    namespace po = boost::program_options;
    po::options_description server_options("Server options");
    server_options.add_options()
        ("inputfile,i", po::value<string>(), "Path of file fo all clients. This options is mandatory for server. ")
        ("count,c", po::value<int32_t>(), "Stop after sending count packets. The default value is 5. ")
        ("timeout,t", po::value<int32_t>(), "Specify a timeout, in seconds. The default value is 3. "
                                            "If this is set to zero, it will never timeout. ")
        ("packetsize,s", po::value<int32_t>(), "Specify the number of data bytes to be sent."
                                               "The default is 28, which is size of UDP header and IP header, "
                                               "value less than this will be ignored. "
                                               "Don't set this value too large which exceeds the limit for "
                                               "the data length imposed by the underlying IPv4 protocol. ");

    po::options_description client_options("Client options");
    client_options.add_options()
        ("listen,l", "Listen for incoming messages. This option is mandatory for client. ");

    po::options_description generic_options("Generic options");
    generic_options.add_options()
        ("help,h", "Display this help message. ")
        ("version,v", "Display version number of this program. ")
        ("port,p", po::value<uint16_t>(), "Specify listen/send port for client/server. "
                                           "This option is mandatory for client. "
                                           "If you don't specify a port for a server, "
                                           "the system will automatically allocate a free port. ");

    all_options.add(generic_options).add(server_options).add(client_options);
}

void OptionParser::parse(int argc, char** argv) {
    using namespace std;
    namespace po = boost::program_options;
    _is_server = _is_client = 0;
    _filename.clear();

    try {
        po::store(parse_command_line(argc, argv, all_options), vm);
        po::notify(vm);

        // print version message
        if (vm.size() == 1 && vm.count("version")) {
            cout << "Version: 1.0" << "\n";
            cout << "Copyright (c) 2015 Jamis Hoo" << "\n";
            cout << "Distributed under the MIT license" << "\n";
            return;
        }

        // print help message
        if (vm.size() == 1 && vm.count("help")) {
            cout << all_options << "\n";
            return;
        }

        // client
        if (vm.size() == 2 && vm.count("listen") && vm.count("port")) {
            _is_client = 1;
            _port = vm["port"].as<uint16_t>();
            return;
        }
        
        // server
        if (vm.count("inputfile") && !vm.count("help") && !vm.count("version") && !vm.count("listen")) {
            _is_server = 1;
            _filename = vm["inputfile"].as<string>();
            if (vm.count("port")) 
                _port = vm["port"].as<uint16_t>();
            else 
                _port = 0;
            if (vm.count("timeout")) 
                _timeout = vm["timeout"].as<int32_t>();
            else
                _timeout = 3;
            if (vm.count("count")) 
                _packet_count = vm["count"].as<int32_t>();
            else 
                _packet_count = 5;
            if (vm.count("packetsize")) 
                _packet_size = vm["packetsize"].as<int32_t>();
            else 
                _packet_size = PACKET_HEADER_SIZE;
            loadClientAddrs();
            return;
        }


        // else invalid options
        if (argc - 1)
            cout << "Invalid option(s)." << "\n\n";
        cout << all_options << "\n";
    } catch (exception& e) {
        cerr << e.what() << "\n";
    }
}

void OptionParser::loadClientAddrs() {
    std::ifstream fin(_filename);    

    std::string ip_s;
    std::string port_s;
    std::array<uint8_t, 4> ip;
    uint16_t port;
    while (fin >> ip_s >> port_s) {
        try {
            ip = text2ip(ip_s);
            int port_tmp = std::stoi(port_s);
            if (port_tmp > std::numeric_limits<uint16_t>::max() || 
                port_tmp < std::numeric_limits<uint16_t>::min())
                throw std::out_of_range(port_s);
            port = port_tmp;
            _client_addrs.emplace_back(ip, port);
        } catch (std::exception& error) {
            std::cerr << error.what() << std::endl;
        }
    }
}


