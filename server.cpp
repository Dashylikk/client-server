#include <iostream>
#include <winsock2.h>
#include <vector>
#include <ctime>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 1052;
const int MAX_ATTEMPTS = 5; // Кількість спроб для вгадування

struct Point {
    int x;
    int y;
};

std::vector<Point> objectPoints; // Множина точок об'єкта
const int OBJECT_SIZE = 10; // Розмір об'єкта

// Функція для вгадування об'єкта
bool GuessObject(Point guess) {
    for (const Point& objectPoint : objectPoints) {
        if (std::abs(guess.x - objectPoint.x) <= OBJECT_SIZE / 2 &&
            std::abs(guess.y - objectPoint.y) <= OBJECT_SIZE / 2) {
            return true; // Об'єкт вгадано
        }
    }
    return false; // Об'єкт не вгадано
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Помилка при ініціалізації Winsock." << std::endl;
        return 1;
    }

    int server_fd, new_socket;
    sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    char buffer[1024] = { 0 };
    char response[1024] = { 0 };

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        WSACleanup();
        return 1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        WSACleanup();
        return 1;
    }


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("bind failed");
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) == -1) {
        perror("listen");
        WSACleanup();
        return 1;
    }

    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < OBJECT_SIZE; ++i) {
        Point point;
        point.x = rand() % 100; // Змініть розмір області за потребою
        point.y = rand() % 100;
        objectPoints.push_back(point);
    }

    std::cout << "Сервер слухає на порту " << PORT << std::endl;

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == -1) {
        perror("accept");
        WSACleanup();
        return 1;
    }

    for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
        int valread = recv(new_socket, buffer, sizeof(buffer), 0);
        if (valread <= 0) {
            std::cerr << "Помилка при отриманні даних від клієнта." << std::endl;
            break;
        }

        buffer[valread] = '\0';
        std::cout << "Клієнт вгадує: " << buffer << std::endl;

        Point guess;
        guess.x = rand() % 100; // Змініть розмір області за потребою
        guess.y = rand() % 100;

        bool isCorrect = GuessObject(guess);
        if (isCorrect) {
            strcpy(response, "Ви вгадали об'єкт!");
        }
        else {
            strcpy(response, "Об'єкт не вгадано.");
        }

        send(new_socket, response, strlen(response), 0);
    }

    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
