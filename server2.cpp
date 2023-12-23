#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <termios.h>
#include <assert.h>
#include <unistd.h>
#include <sstream>
#include <stdio.h>
#include <malloc.h>

using namespace std;

long long getMemoryValue(const string &key)
{
    ifstream meminfo("/proc/meminfo");
    string line;

    while (getline(meminfo, line))
    {
        istringstream iss(line);
        string currentKey;
        long long value;

        iss >> currentKey >> value;

        if (currentKey == key + ":")
        {
            return value;
        }
    }

    return -1;
}

void connection_handler(int socket)
{
    char client_message[512];
    char msgBuff[512];
    memset(&msgBuff, 0, 512);
    size_t message_size = 0;
    struct termios oldattr, newattr;
    int dir;

    const long long totalMemory = getMemoryValue("MemTotal");
    const long long freeMemory = getMemoryValue("MemFree");
    const long long totalSwap = getMemoryValue("SwapTotal");
    const long long freeSwap = getMemoryValue("SwapFree");

    if (totalMemory != -1 && freeMemory != -1 && totalSwap != -1 && freeSwap != -1)
    {
    }
    else
    {
        cerr << "Ошибка при получении информации о памяти." << endl;
    }

    message_size = recv(socket, msgBuff, sizeof(msgBuff) - 1, 0);
    msgBuff[message_size] = '\0';
    cout << "[Server] Client message accepted" << endl;

    if (msgBuff[1] == '1')
    {
        while ((message_size = recv(socket, client_message, sizeof(client_message) - 1, 0)) > 0)
        {

            const long long totalMemory = getMemoryValue("MemTotal");
            const long long freeMemory = getMemoryValue("MemFree");
            const long long totalSwap = getMemoryValue("SwapTotal");
            const long long freeSwap = getMemoryValue("SwapFree");

            time_t currentTime = time(nullptr);
            tm *localTime = localtime(&currentTime);
            char timeString[100];
            strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);
            string timeStr(timeString);

            client_message[message_size] = '\0';
            cout << "[Server] Client message accepted" << endl;
            cout << "[Server] Client message: " << client_message << endl;

            string fullMsg = (string)timeString;
            if (msgBuff[0] == '3')
            {
                return;
            }
            else if (msgBuff[0] == '1')
            {

                char str[100];

                double physicalMemoryUsage = ((totalMemory - freeMemory) / static_cast<double>(totalMemory)) * 100.0;
                sprintf(str, "%f", physicalMemoryUsage);
                fullMsg = " :: Процент используемой физической памяти: " + string(str) + "%";
            }
            else if (msgBuff[0] == '2')
            {
                char str[100];

                double virtualMemoryUsage = ((totalSwap - freeSwap) / static_cast<double>(totalSwap)) * 100.0;
                sprintf(str, "%f", virtualMemoryUsage);
                fullMsg = (string)timeString + " :: Процент используемой виртуальной памяти: " + string(str) + "%";
            }
            else
            {
                char str[100];
                char str1[100];

                double physicalMemoryUsage = ((totalMemory - freeMemory) / static_cast<double>(totalMemory)) * 100.0;
                double virtualMemoryUsage = ((totalSwap - freeSwap) / static_cast<double>(totalSwap)) * 100.0;
                sprintf(str, "%f", physicalMemoryUsage);
                sprintf(str1, "%f", virtualMemoryUsage);

                fullMsg = (string)timeString + " :: Процент используемой физической памяти: " + string(str) + "%" + " :: Процент используемой виртуальной памяти: " + string(str1) + "%";
            }

            if (write(socket, fullMsg.c_str(), fullMsg.size()) == -1)
            {
                cout << "[Client] " << fullMsg << " Message sending failed" << endl;
                return;
            }
            cout << "[Server] " << fullMsg << endl
                 << endl;
            cout << "============================" << endl
                 << endl;
            cout.flush();

            memset(&client_message, 0, 512);
        }
    }
    else
    {
        while ((message_size = recv(socket, client_message, sizeof(client_message) - 1, 0)) > 0)
        {

            const long long totalMemory = getMemoryValue("MemTotal");
            const long long freeMemory = getMemoryValue("MemFree");
            const long long totalSwap = getMemoryValue("SwapTotal");
            const long long freeSwap = getMemoryValue("SwapFree");

            client_message[message_size] = '\0';
            cout << "[Server] Client message accepted" << endl;
            cout << "[Server] Client message: " << client_message << endl;
            time_t currentTime = time(nullptr);
            tm *localTime = localtime(&currentTime);
            char timeString[100];
            strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);
            string timeStr(timeString);

            string fullMsg = (string)timeString;
            if (msgBuff[0] == '3')
            {
                return;
            }
            else if (msgBuff[0] == '1')
            {
                char str[100];

                double physicalMemoryUsage = ((totalMemory - freeMemory) / static_cast<double>(totalMemory)) * 100.0;
                sprintf(str, "%f", physicalMemoryUsage);
                fullMsg = " :: Процент используемой физической памяти: " + string(str) + "%";
            }
            else if (msgBuff[0] == '2')
            {
                char str[100];

                double virtualMemoryUsage = ((totalSwap - freeSwap) / static_cast<double>(totalSwap)) * 100.0;
                sprintf(str, "%f", virtualMemoryUsage);
                fullMsg = " :: Процент используемой виртуальной памяти: " + string(str) + "%";
            }
            else
            {
                char str[100];
                char str1[100];

                double physicalMemoryUsage = ((totalMemory - freeMemory) / static_cast<double>(totalMemory)) * 100.0;
                double virtualMemoryUsage = ((totalSwap - freeSwap) / static_cast<double>(totalSwap)) * 100.0;
                sprintf(str, "%f", physicalMemoryUsage);
                sprintf(str1, "%f", virtualMemoryUsage);

                fullMsg = " :: Процент используемой физической памяти: " + string(str) + "%" + " :: Процент используемой виртуальной памяти: " + string(str1) + "%";
            }
            if (write(socket, fullMsg.c_str(), fullMsg.size()) == -1)
            {
                cout << "[Client] " << fullMsg << " Message sending failed" << endl;
                return;
            }
            cout << "[Server] " << fullMsg << endl
                 << endl;
            cout << "============================" << endl
                 << endl;
            cout.flush();

            memset(&client_message, 0, 512);
        }
    }
    memset(&msgBuff, 0, 512);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " <port>\n";

        return 1;
    }
    unsigned short int PORT;
    PORT = atoi(argv[1]);

    int listener, client_socket;
    socklen_t client_len;

    struct sockaddr_in server_address
    {
    };

    memset(&server_address, 0, sizeof(server_address));

    listener = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_aton("127.0.0.1", &server_address.sin_addr) == 0)
    {
        cout << "[Server] Invalid IP address" << endl;
        return -1;
    }

    if (bind(listener, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        cout << "[Server] Binding failed" << endl;
        return -1;
    }
    cout << "[Server] All setting are done" << endl;
    cout << "[Server] Server enabled" << endl;

    if (listen(listener, 100) == -1)
    {
        cout << "[Server] Listening failed" << endl;
        return -1;
    }
    cout << "Сервер 2: Задание 1: \nпроцент используемой физической памяти\n";
    cout << "Сервер 2: Задание 2: \nпроцент используемой виртуальной памяти\n";
    cout << "[Server] Waiting for connection..." << endl;

    for (;;)
    {
        client_socket = accept(listener, (struct sockaddr *)&server_address, &client_len);
        cout << "[Server] Connection accepted" << endl
             << endl;
        cout << "----------------------------" << endl
             << endl;

        int *new_socket = new int;
        *new_socket = client_socket;

        thread handling_thread(connection_handler, *new_socket);
        handling_thread.detach();
    }
}
