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


using namespace std;
using namespace cv;
const int BUFF_SIZE = 65536;
const double TIMEOUT_SECONDS = 0.5;
char packett[BUFF_SIZE];
char buffer[BUFF_SIZE];
int imageW=640;
int imageH=480;
struct timeval timeout;
int main(){
    std::string _Udp_Port = "9999";
    std::string _Host_Ip = "127.0.0.1";
    int Udp_Port=std::stoi(_Udp_Port);
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    timeout.tv_sec = 5;//static_cast<long>(TIMEOUT_SECONDS);
    timeout.tv_usec =0;
    // Set the socket to non-blocking mode
    int flags = fcntl(server_socket, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error getting socket flags" << std::endl;
        close(server_socket);
        return 1;
    }
    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);
    int rcvbuf_size = BUFF_SIZE;
    std::cout<<"TIMEVAL SIZE"<<sizeof(struct timeval)<<std::endl;

    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout,sizeof(struct timeval));

    struct sockaddr_in socket_address,cliaddr;

    memset(&socket_address,0,sizeof(socket_address));
    memset(&cliaddr,0,sizeof(cliaddr));

    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(Udp_Port);

    std::cout<<_Host_Ip<<std::endl;
    socket_address.sin_addr.s_addr=inet_addr(_Host_Ip.c_str());

    // int flags = fcntl(server_socket, F_GETFL, 0);
    // fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);
    
    bind(server_socket, (const struct sockaddr*)&socket_address, sizeof(socket_address));

    fd_set write_set;

    FD_ZERO(&write_set);
    FD_SET(server_socket, &write_set);
    listen(server_socket, 1);  
    fflush(stdout); 
    cv::VideoCapture capp(0);
    cv::Mat _template;
    _template=cv::Mat::zeros(1,1,CV_8UC3);

    auto lastSend = std::chrono::steady_clock::now();
    std::vector<uchar> buffered;

    u_long nbio = 1;
    ioctl(server_socket, FIONBIO, &nbio);
    while (true)
    {
        timeout.tv_sec = 5;//static_cast<long>(TIMEOUT_SECONDS);
        timeout.tv_usec =0;// static_cast<long>((TIMEOUT_SECONDS - timeout.tv_sec) * 1e6);
        int t = select(0, &write_set, NULL, NULL, &timeout);
        int select_result = select(server_socket + 1, nullptr, &write_set, nullptr, &timeout);
        if (select_result == 0) {
            std::cerr << "Connection timed out." << std::endl;
            continue; // Retry the connection in the next iteration
        } else if (select_result < 0) {
            std::cerr << "Error in select." << std::endl;
            break; // Exit the loop on select error
        }
        try{
            fd_set tmp_fds = write_set; 
            socklen_t addr_size = sizeof(cliaddr);
            int bytes_received = select(server_socket + 1, &tmp_fds, nullptr, nullptr, nullptr);
            std::cout<<"Bytes Received ->"<<bytes_received<<std::endl;            
            if (FD_ISSET(server_socket, &tmp_fds)) 
            {
                int a = recvfrom(server_socket,(char *)buffer, BUFF_SIZE, 0, (struct sockaddr*)&cliaddr, &addr_size);
                //int recv_bytes = recv(server_socket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
                
            if (bytes_received > 0)
            {
                std::string msg(buffer, bytes_received);
                std::cout << "Connection String: " << inet_ntoa(cliaddr.sin_addr) << std::endl;

                std::string old_buffer;

                while (true)
                {
                    
                    cv::Mat main_frame; 
                    capp>>main_frame;
                    cv::Mat frame = main_frame.clone();
                    
                    //cv::Size targetSize(640, 480);
                    cv::Size targetSize(640 / imageSize, 480 / imageSize);

                    //cv::resize(frame,frame, targetSize);
                    cv::resize(frame, frame,targetSize);
                    cv::imencode(".jpg", frame, buffered, {cv::IMWRITE_JPEG_QUALITY, 80});
                    std::string buffer_str(buffered.begin(), buffered.end());

                    size_t boy = buffer_str.size();
                    auto now = std::chrono::steady_clock::now();
                    bool sended = false;
                    double now_lastSend = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSend).count();
                    
                    if (now_lastSend > 30) // saniyede 10 kere gonder
                    {   
                        ssize_t bytes_sent = sendto(server_socket, buffer_str.data(), buffer_str.size(), MSG_CONFIRM, (struct sockaddr*)&cliaddr,  sizeof(struct timeval));
                        if (bytes_sent > 0)
                        {
                            sended = true;
                        }
                    }
                     if (sended)
                    {
                        eski_buffer = buffer_str;
                        std::string gelen;
                        struct sockaddr_in adresi;
                        bool durum;
                        std::tie(gelen, adresi, durum) = Responser(server_socket,tmp_fds);
                        lastSend = std::chrono::steady_clock::now();
                        auto nower= std::chrono::steady_clock::now();
                        if (gelen.empty() || gelen == "True")
                        {
                            continue;
                        }
                        if (!durum)
                        {
                            break;
                        }
                        if (gelen.substr(0, 5) == "received")
                        {   
                            std::string muhtemelFlag = gelen.substr(7);
                            framer.lastFrame = main_frame.clone();
                            framer.lastCleanFrame = main_frame.clone(); // temizden alınacak
                        }
                    else
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
        }
        catch(const std::exception& e){
            std::cout<<"Error"<<std::endl;
        }
    }
            }
    return 0;
}}