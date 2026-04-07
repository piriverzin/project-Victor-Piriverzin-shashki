#include "http_server.h"
#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>

CheckersGame game;

HTTPServer::HTTPServer(int port) : port(port), running(false) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Ошибка создания сокета" << std::endl;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

HTTPServer::~HTTPServer() {
    stop();
    close(server_fd);
}

void HTTPServer::addHandler(const std::string& path, std::function<std::string(const std::string&)> handler) {
    handlers[path] = handler;
}

std::string HTTPServer::getLocalIP() {
    std::string ip = "127.0.0.1";
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        return ip;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (ifa->ifa_addr->sa_family == AF_INET && (ifa->ifa_flags & IFF_UP)) {
            if (strcmp(ifa->ifa_name, "lo") != 0) {
                struct sockaddr_in *addr = (struct sockaddr_in*)ifa->ifa_addr;
                ip = inet_ntoa(addr->sin_addr);
                break;
            }
        }
    }
    
    freeifaddrs(ifaddr);
    return ip;
}

std::string HTTPServer::getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html; charset=utf-8";
    if (path.find(".css") != std::string::npos) return "text/css; charset=utf-8";
    if (path.find(".js") != std::string::npos) return "application/javascript; charset=utf-8";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos) return "image/jpeg";
    if (path.find(".ico") != std::string::npos) return "image/x-icon";
    return "text/plain";
}

std::string HTTPServer::readFile(const std::string& path) {
    std::string fullPath = "public" + path;
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string HTTPServer::createResponse(int statusCode, const std::string& contentType, const std::string& body) {
    std::stringstream response;
    response << "HTTP/1.1 " << statusCode << " OK\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    response << "Access-Control-Allow-Headers: Content-Type\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;
    return response.str();
}

std::string HTTPServer::parseRequestBody(const std::string& request) {
    size_t bodyPos = request.find("\r\n\r\n");
    if (bodyPos != std::string::npos) {
        return request.substr(bodyPos + 4);
    }
    return "";
}

void HTTPServer::handleRequest(int client_fd) {
    char buffer[65536] = {0};
    read(client_fd, buffer, sizeof(buffer) - 1);
    
    std::string request(buffer);
    std::string method, path;
    
    std::istringstream requestStream(request);
    requestStream >> method >> path;
    
    std::string response;
    
    if (method == "OPTIONS") {
        response = createResponse(200, "text/plain", "");
    } else if (path == "/api/game_state") {
        response = createResponse(200, "application/json", game.getGameStateJSON());
    } else if (path == "/api/move" && method == "POST") {
        std::string body = parseRequestBody(request);
        size_t fromRowPos = body.find("fromRow=");
        size_t fromColPos = body.find("fromCol=");
        size_t toRowPos = body.find("toRow=");
        size_t toColPos = body.find("toCol=");
        
        if (fromRowPos != std::string::npos && fromColPos != std::string::npos &&
            toRowPos != std::string::npos && toColPos != std::string::npos) {
            
            int fromRow = std::stoi(body.substr(fromRowPos + 8, 1));
            int fromCol = std::stoi(body.substr(fromColPos + 8, 1));
            int toRow = std::stoi(body.substr(toRowPos + 6, 1));
            int toCol = std::stoi(body.substr(toColPos + 6, 1));
            
            bool success = game.makeMove(fromRow, fromCol, toRow, toCol);
            response = createResponse(200, "application/json", "{\"success\":" + std::string(success ? "true" : "false") + "}");
        } else {
            response = createResponse(400, "application/json", "{\"error\":\"Invalid parameters\"}");
        }
    } else if (path == "/api/select" && method == "POST") {
        std::string body = parseRequestBody(request);
        size_t rowPos = body.find("row=");
        size_t colPos = body.find("col=");
        
        if (rowPos != std::string::npos && colPos != std::string::npos) {
            int row = std::stoi(body.substr(rowPos + 4, 1));
            int col = std::stoi(body.substr(colPos + 4, 1));
            game.selectPiece(row, col);
            response = createResponse(200, "application/json", "{\"success\":true}");
        } else {
            response = createResponse(400, "application/json", "{\"error\":\"Invalid parameters\"}");
        }
    } else if (path == "/api/reset" && method == "POST") {
        game.resetGame();
        response = createResponse(200, "application/json", "{\"success\":true}");
    } else if (path == "/api/move_selected" && method == "POST") {
        std::string body = parseRequestBody(request);
        size_t rowPos = body.find("row=");
        size_t colPos = body.find("col=");
        
        if (rowPos != std::string::npos && colPos != std::string::npos) {
            int row = std::stoi(body.substr(rowPos + 4, 1));
            int col = std::stoi(body.substr(colPos + 4, 1));
            game.moveSelected(row, col);
            response = createResponse(200, "application/json", "{\"success\":true}");
        } else {
            response = createResponse(400, "application/json", "{\"error\":\"Invalid parameters\"}");
        }
    } else {
        if (path == "/") path = "/index.html";
        std::string content = readFile(path);
        if (!content.empty()) {
            response = createResponse(200, getContentType(path), content);
        } else {
            response = createResponse(404, "text/plain", "404 Not Found");
        }
    }
    
    send(client_fd, response.c_str(), response.length(), 0);
    close(client_fd);
}

void HTTPServer::start() {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Ошибка привязки порта " << port << std::endl;
        return;
    }
    
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Ошибка прослушивания" << std::endl;
        return;
    }
    
    running = true;
    std::string localIP = getLocalIP();
    
    std::cout << "\n=========================================" << std::endl;
    std::cout << "🎮 Сервер шашек запущен!" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "📍 Локальный доступ: http://localhost:" << port << std::endl;
    std::cout << "🌐 Сеть: http://" << localIP << ":" << port << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "📱 Подключитесь с других устройств по IP" << std::endl;
    std::cout << "🔄 Нажмите Ctrl+C для остановки" << std::endl;
    std::cout << "=========================================\n" << std::endl;
    
    while (running) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
            handleRequest(client_fd);
        }
    }
}

void HTTPServer::stop() {
    running = false;
    close(server_fd);
}