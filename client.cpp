#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " <port>\n";

        return 1;
    }

    unsigned short int PORT;
    PORT = atoi(argv[1]);

    cout << "You connect to port: " << PORT << endl;

    int sockfd;
    char buffer[1024] = {0};
    struct sockaddr_in server_address
    {
    };

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cout << "[Client] Connection failed" << endl;
        return -1;
    }
    cout << "[Client] All setting are done" << endl;
    cout << "[Client] Succefully connected to server" << endl
         << endl;
    cout << "----------------------------" << endl
         << endl;

    while (true)
    {
        string client_request;
        cout << "Сервер 1: Задание 1: \nколичество жестких дисков и тип файловой системы каждого\n";
        cout << "Сервер 1: Задание 2: \nколичество логических процессоров\n";
        cout << "Сервер 2: Задание 1: \nпроцент используемой физической памяти\n";
        cout << "Сервер 2: Задание 2: \nпроцент используемой виртуальной памяти\n";
        cout << "[Client] Enter message(0 Полный ответ, 1 Задание, 2 Задание, 3 Отключение): ";
        getline(cin, client_request);
        if (client_request == "3")
        {
            write(sockfd, client_request.c_str(), client_request.size());
            close(sockfd);
            cout << endl
                 << "[Client] Client exited" << endl;
            return 0;
        }
        else if (client_request != "1" && client_request != "2" && client_request != "0")
        {
            cout << endl
                 << "[Client] Invalid msg,  Use: 0, 1, 2, 3" << endl;
            return 0;
        }
        string msg_type;
        cout << "[Client] Enter message(1 Каждую секунду, 2 Каждый ввод): ";
        getline(cin, msg_type);

        if (write(sockfd, (client_request + msg_type).c_str(), (client_request + msg_type).size()) == -1)
        {
            cout << "[Client] Message sending failed" << endl;
        }
        cout << "[Client] Message sent to server" << endl;

        if (msg_type == "1")
        {

            while (true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if (write(sockfd, "ping", 5) == -1)
                {
                    cout << "[Client] Message sending failed" << endl;
                    return 0;
                }
                cout << "[Client] Message sent to server" << endl;
                memset(&buffer, 0, 1024);
                read(sockfd, buffer, 1024);

                cout << "[Client] Server message: " << buffer << endl
                     << endl;
                cout << "============================" << endl
                     << endl;
                cout.flush();
            }
        }
        else if (msg_type == "2")
        {
            while (true)
            {
                cout << "[Client] Enter any message: ";
                getline(cin, msg_type);
                cout << (int)msg_type[0] << endl;
                if (msg_type == "\n" || (int)msg_type[0] == 0)
                    msg_type = "q";
                if (write(sockfd, (msg_type).c_str(), (msg_type).size()) == -1)
                {
                    cout << "[Client] Message sending failed" << endl;
                    return 0;
                }
                cout << "[Client] Message sent to server" << endl;
                memset(&buffer, 0, 1024);
                read(sockfd, buffer, 1024);

                cout << "[Client] Server message: " << buffer << endl
                     << endl;
                cout << "============================" << endl
                     << endl;
                cout.flush();
            }
        }
        else
        {
            cout << endl
                 << "[Client] Invalid msg, Use: 1 or 2" << endl;
            return 0;
        }
    }
}
