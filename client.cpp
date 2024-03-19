#include <iostream>
#include <winsock2.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 1052;
const int MAX_COMMAND_LENGTH = 255;

struct Message {
    char headerLength; // Довжина заголовка (завжди 1, якщо немає команд)
    std::string command; // Команди розділені ';'
    std::string data; // Дані повідомлення
};

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Помилка при ініціалізації Winsock." << std::endl;
        return 1;
    }

    int client_socket;
    sockaddr_in server_addr;
    char buffer[MAX_COMMAND_LENGTH] = { 0 };

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    Message message;
    message.headerLength = 1; // Довжина заголовка завжди 1, якщо немає команд
    message.command = "Команда1; Команда2";
    message.data = "Дані повідомлення";

    // Серіалізація повідомлення перед відправленням
    std::string serializedMessage;
    serializedMessage.push_back(message.headerLength);
    serializedMessage += message.command;
    serializedMessage += message.data;

    if (send(client_socket, serializedMessage.c_str(), serializedMessage.length(), 0) == -1) {
        perror("send");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Отримання відповіді від сервера і розшифрування
    memset(buffer, 0, sizeof(buffer));
    int valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread <= 0) {
        std::cerr << "Сервер відключився." << std::endl;
    }
    else {
        std::cout << "Відповідь сервера: " << buffer << std::endl;
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}
