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


int main(int argc, char** argv) {
    using namespace std;

    OptionParser parser;
    parser.parse(argc, argv);
    
    
    if (parser.isServer()) {
        cout << "Is server" << "\n";
        cout << parser.port() << "\n";
        cout << parser.configfile() << "\n";
    } else if (parser.isClient()) {
        cout << "Is client ";
        cout << parser.port() << "\n";
        
        try {
            Client client(parser.port());
            client.listen();
        } catch (std::exception& error) {
            std::cout << error.what() << std::endl;
        }
    }

}
