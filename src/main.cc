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
 *  Description: main function
 *****************************************************************************/
#include <iostream>
#include "parser.h"
#include "client.h"
#include "server.h"


int main(int argc, char** argv) {
    using namespace std;

    OptionParser parser;
    // parse arguments
    parser.parse(argc, argv);
    
    // if this is server
    if (parser.isServer()) {
        try {
            // set server runtime options
            Server server(parser.clientAddrs());
            server.setSendCount(parser.packetCount());
            server.setTimeout(parser.timeout());
            server.setPacketSize(parser.packetSize());
            server.setPort(parser.port());
            
            // start measuring
            server.measure();

            // show result
            server.showStat();
        } catch (std::exception& error) {
            std::cerr << error.what() << std::endl;
        }
    // else if this is client
    } else if (parser.isClient()) {
        try {
            // set client runtime options
            Client client(parser.port());

            // start listening
            client.listen();
        } catch (std::exception& error) {
            std::cerr << error.what() << std::endl;
        }
    }

}
