#include <iostream>
#include "webserver.h"

int main(int argc, char *argv[]) {
    try {
        std::string::size_type sz;
        int port = std::stoi(argv[1], &sz);
        Webserver * server = new Webserver(port, "site");
        
        server->start();
    } catch(...) {
        std::cout << "[ERROR] Invalid port entry." << std::endl;
    }
    
    return 0;
}