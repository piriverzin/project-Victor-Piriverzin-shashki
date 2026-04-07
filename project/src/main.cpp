#include "http_server.h"
#include <iostream>
#include <csignal>

HTTPServer* server = nullptr;

void signalHandler(int signum) {
    std::cout << "\n\n🛑 Остановка сервера..." << std::endl;
    if (server) {
        server->stop();
    }
    exit(signum);
}

int main() {
    signal(SIGINT, signalHandler);
    
    server = new HTTPServer(8080);
    server->start();
    
    delete server;
    return 0;
}
