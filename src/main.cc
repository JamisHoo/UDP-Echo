/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: UDP-Echo
 *  Filename: main.cc 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hjm211324@gmail.com
 *  Date: May  1, 2015
 *  Time: 18:56:40
 *  Description: 
 *****************************************************************************/
#include <iostream>
#include "parser.h"
#include "client.h"
#include "server.h"


int main(int argc, char** argv) {
    using namespace std;

    OptionParser parser;
    parser.parse(argc, argv);
    
    
    if (parser.isServer()) {
        cout << "Is server" << "\n";

        try {
            Server server(parser.clientAddrs());
            std::cout << "sendcount == " << parser.packetCount() << std::endl;
            std::cout << "timeout == " << parser.timeout() << std::endl;
            std::cout << "packetsize == " << parser.packetSize() << std::endl;
            std::cout << "port == " << parser.port() << std::endl;
            server.setSendCount(parser.packetCount());
            server.setTimeout(parser.timeout());
            server.setPacketSize(parser.packetSize());
            server.setPort(parser.port());
            server.measure();
        } catch (std::exception& error) {
            std::cerr << error.what() << std::endl;
        }

    } else if (parser.isClient()) {
        cout << "Is client ";
        cout << parser.port() << "\n";
        
        try {
            Client client(parser.port());
            client.listen();
        } catch (std::exception& error) {
            std::cerr << error.what() << std::endl;
        }
    }

}
