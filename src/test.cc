#include <iostream>
#include <cstring>
#include "socket.h"

int main(int argc, char** argv) {
    using namespace std;

    if (argc == 2 && !strcmp(argv[1], "s")) {
        socket_t sockfd = udp_socket();

        UdpSocketAddr addr("192.168.1.100", 10000);

        if (udp_bind(sockfd, addr) == -1) {
            std::cout << "Bind error!" << std::endl;
            return 1;
        }
        
        string message = "hello";

        addr.setPort(8888);
        
        udp_send(sockfd, message, addr);
        
        memset(&addr, 0x00, sizeof(addr));


        udp_recv(sockfd, message, addr);
        std::cout << message << std::endl;
        
    } else {
        std::cout << "Unknown arguments." << std::endl;
    }
}
