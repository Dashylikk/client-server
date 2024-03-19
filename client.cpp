#include <iostream>
#include <winsock2.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 1052;
const int MAX_COMMAND_LENGTH = 255;

struct Message {
    char headerLength; // ������� ��������� (������ 1, ���� ���� ������)
    std::string command; // ������� ������� ';'
    std::string data; // ��� �����������
};

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "������� ��� ����������� Winsock." << std::endl;
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
    message.headerLength = 1; // ������� ��������� ������ 1, ���� ���� ������
    message.command = "�������1; �������2";
    message.data = "��� �����������";

    // ���������� ����������� ����� ������������
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

    // ��������� ������ �� ������� � �������������
    memset(buffer, 0, sizeof(buffer));
    int valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread <= 0) {
        std::cerr << "������ ����������." << std::endl;
    }
    else {
        std::cout << "³������ �������: " << buffer << std::endl;
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}
