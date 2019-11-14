#pragma once
#include <iostream>
using namespace std;
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
class Client
{
    private:
        int sock;
        //服务端的IP和端口号
        string peer_ip;
        int peer_port;
    public:
        Client(string _ip,int _port)
            :peer_ip(_ip)
             ,peer_port(_port)
    {
        sock=-1;
    }
        void InitClient()
        {
            sock=socket(AF_INET,SOCK_DGRAM,0);
            if(sock<0)
            {
                cerr<<"socket error"<<endl;
                exit(2);
            }
        }
        void Run()
        {
            bool quit=false;
            string message;
            char buf[1024];
            while(!quit)
            {
                struct sockaddr_in peer;
                //先接收消息再发
                peer.sin_family=AF_INET;
                peer.sin_addr.s_addr=inet_addr(peer_ip.c_str());
                peer.sin_port=htons(peer_port);
                socklen_t len=sizeof(peer);
                cout<<"Please Enter# ";
                cin>>message;
                sendto(sock,message.c_str(),message.size(),0,(struct sockaddr*)&peer,len);
                ssize_t s= recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&peer,&len);
                if(s>0)
                {
                    buf[s]=0;
                    cout<<"server echo# "<<buf<<endl;
                }
            }
        }
        ~Client()
        {}
};
