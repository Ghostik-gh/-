#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <termios.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

string getDiskInfo()
{
    ostringstream result;

    string command = "lsblk --output NAME,FSTYPE,SIZE,TYPE,MOUNTPOINTS";
    FILE *pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        result << "Ошибка при выполнении команды.\n";
        return result.str();
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result << buffer;
    }

    pclose(pipe);

    result << "\n";
    return result.str();
}

string getCPUInfo()
{
    ifstream cpuinfo("/proc/cpuinfo");
    string line;
    int processorCount = 0;
    while (getline(cpuinfo, line))
    {
        if (line.find("processor") != string::npos)
        {
            processorCount++;
        }
    }

    cpuinfo.close();
    return to_string(processorCount);
}

string getEnvVar(string const &key)
{
    char *val = getenv(key.c_str());
    return val == NULL ? string("") : string(val);
}

void connection_handler(int socket)
{
    char client_message[1024];
    char msgBuff[1024];
    memset(&msgBuff, 0, 1024);
    size_t message_size = 0;
    struct termios oldattr, newattr;
    int dir;

    message_size = recv(socket, msgBuff, sizeof(msgBuff) - 1, 0);
    msgBuff[message_size] = '\0';
    cout << "[Server] Client message accepted" << endl;

    if (msgBuff[1] == '1')
    {
        while ((message_size = recv(socket, client_message, sizeof(client_message) - 1, 0)) > 0)
        {
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
                fullMsg = " :: " + getDiskInfo() + "\n";
            }
            else if (msgBuff[0] == '2')
            {
                fullMsg = " :: Количество логических процессоров = " + getCPUInfo() + "\n";
            }
            else
            {
                fullMsg = " :: Количество логических процессоров = " + getCPUInfo() + "\n" + getDiskInfo() + "\n";
            }

            if (write(socket, fullMsg.c_str(), fullMsg.size()) == -1)
            {
                cout << "[Client] " << fullMsg << " Message sending failed" << endl;
                return;
            }
            cout << "[Server] " << fullMsg << " Message sent to client" << endl
                 << endl;
            cout << "============================" << endl
                 << endl;
            cout.flush();

            memset(&client_message, 0, 1024);
        }
    }
    else
    {
        while ((message_size = recv(socket, client_message, sizeof(client_message) - 1, 0)) > 0)
        {
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

                fullMsg = " :: " + getDiskInfo() + "\n";
            }
            else if (msgBuff[0] == '2')
            {
                fullMsg = " :: Количество логических процессоров = " + getCPUInfo() + "\n";
            }
            else
            {
                fullMsg = " :: Количество логических процессоров = " + getCPUInfo() + "\n" + getDiskInfo() + "\n";
            }
            if (write(socket, fullMsg.c_str(), fullMsg.size()) == -1)
            {
                cout << "[Client] " << fullMsg << " Message sending failed" << endl;
                return;
            }
            cout << "[Server] " << fullMsg << " Message sent to client" << endl
                 << endl;
            cout << "============================" << endl
                 << endl;
            cout.flush();

            memset(&client_message, 0, 1024);
        }
    }
    memset(&msgBuff, 0, 1024);
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
    cout << "Сервер 1: Задание 1: \nколичество жестких дисков и тип файловой системы каждого\n";
    cout << "Сервер 1: Задание 2: \nколичество логических процессоров\n";
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
