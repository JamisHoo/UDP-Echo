/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: 
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

void OptionParser::initialize() {
    using namespace std;
    namespace po = boost::program_options;
    po::options_description server_options("Server options");
    server_options.add_options()
        ("input-file,i", po::value<string>(), "path of file fo all clients. ");

    po::options_description client_options("Client options");
    client_options.add_options()
        ("listen,l", "listen for incoming messages. ");

    po::options_description generic_options("Generic options");
    generic_options.add_options()
        ("help,h", "display this help message. ")
        ("version,v", "display version number of this program. ")
        ("port,p", po::value<uint16_t>(), "specify listen/send port for client/server. ");

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
        
        // server
        if (vm.size() == 2 && vm.count("input-file") && vm.count("port")) {
            _is_server = 1;
            _filename = vm["input-file"].as<string>();
            _port = vm["port"].as<uint16_t>();
            return;
        }

        // client
        if (vm.size() == 2 && vm.count("listen") && vm.count("port")) {
            _is_client = 1;
            _port = vm["port"].as<uint16_t>();
            return;
        }

        // else invalid options
        cout << "Invalid option(s)." << "\n\n";
        cout << all_options << "\n";
    } catch (exception& e) {
        cout << e.what() << "\n";
    }
}

