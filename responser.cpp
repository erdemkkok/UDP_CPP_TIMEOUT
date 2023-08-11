#include <iostream>
#include "nlohmann/json.hpp"
#include <sys/socket.h>
#include <opencv2/opencv.hpp> 
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/select.h>
std::tuple<std::string, sockaddr_in, bool> Responser(int server_socket,fd_set tmp_fds)
    {
        sockaddr_in clientt{};
        //socklen_t addr_size = sizeof(clientt);
        socklen_t addr_size = sizeof(struct timeval);
        timeout.tv_sec = 0; // Seconds
        timeout.tv_usec = 500000; // Microseconds (0.5 seconds)
        try
        {   std::cout<<"dene"<<addr_size<<"---"<<std::endl;
            int bytes_received = select(server_socket + 1, &tmp_fds, nullptr, nullptr, &timeout);
            if (FD_ISSET(server_socket, &tmp_fds)) 
            {
                int bytes_received = recvfrom(server_socket, packett, BUFF_SIZE, 0, (struct sockaddr*)&clientt, &addr_size);
                std::cout<<"Response aldim"<<std::endl;
                //int bytes_received = recv(server_socket, packett, sizeof(packett) - 1, MSG_DONTWAIT);
            
            cout<<"RESPONSE"<<bytes_received<<endl;
            if (bytes_received > 0)
            {   
                return {std::string(packett, bytes_received), clientt, true};
            }
            else
            {
                return {"", clientt, true};
            }}
        }
        catch (const std::exception& e)
        {   cout<<"exception"<<endl;
            std::cerr << "Exception: " << e.what() << std::endl;
            return {"", clientt, false};
        }
    }