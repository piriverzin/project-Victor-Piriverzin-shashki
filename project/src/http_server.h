#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <functional>
#include <map>

class HTTPServer {
private:
    int server_fd;
    int port;
    bool running;
    std::map<std::string, std::function<std::string(const std::string&)>> handlers;
    
    std::string getContentType(const std::string& path);
    std::string readFile(const std::string& path);
    std::string createResponse(int statusCode, const std::string& contentType, const std::string& body);
    void handleRequest(int client_fd);
    std::string parseRequestBody(const std::string& request);
    
public:
    HTTPServer(int port = 8080);
    ~HTTPServer();
    
    void addHandler(const std::string& path, std::function<std::string(const std::string&)> handler);
    void start();
    void stop();
    std::string getLocalIP();
};

#endif
