#pragma once
#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <map>
using namespace std;
map<string,string> dict;//词典
class Server
{
    private:
        int sock;//套接字
        int port;//端口号
        string ip;
    public:
        Server(string _ip="127.0.0.1",int _port=8080)
            :sock(-1)
             ,port(_port)
             ,ip(_ip)
    {}
        void InitServer()
        {
            dict.insert(pair<string,string>("apple","苹果"));
            dict.insert(pair<string,string>("banana","香蕉"));
            //           ipv4     UDP
            sock=socket(AF_INET,SOCK_DGRAM,0);
            if(sock<0)
            {
                cerr<<"socket srror"<<endl;
                exit(2);//退出码从2开始
            }
            cout<<"init ....socket create success :"<<sock<<endl;//打印套接字
            struct sockaddr_in local;//定义sockaddr_in结构体变量
            local.sin_family=AF_INET;
            local.sin_port=htons(port);//将主机转为网络，端口号是32位bit，因此是s
            local.sin_addr.s_addr=inet_addr(ip.c_str());
            //地址转换函数，将char*类型转换为in_addr_t类型，但是我们这里使用的是string类型的ip，因此将c++风格的string类型要转为c语言类型的，使用c_str()
            if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
            {
                cerr<<"bind error"<<endl;
                exit(3);
            }
            cout<<"init socket bind success :"<<sock<<endl;
        }
        //启动服务器
        void Start()
        {
            char buf[1024];
            for(;;)
            {
                struct sockaddr_in peer;//对方（客户端）的地址
                socklen_t len=sizeof(peer);
                ssize_t s=recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&peer,&len);
                //读消息，即收消息，返回值代表收到了多少个字节，第3个参数代表期望读多长的数据
                if(s>0)
                {
                    buf[s]=0;
                    string key=buf;
                    string value;
                    auto it=dict.find(key);
                    if(it!=dict.end())
                    {
                        value=dict[key];
                    }
                    else
                        value="unknow";
                    cout<<"client#"<<buf<<endl;
                    sendto(sock,value.c_str(),value.size(),0,(struct sockaddr*)&peer,len);//向peer发消息,strlen不用+1，因为\0是结束的标志，不是字符串的内容。
                }
            }
        }
        ~Server()
        {

        }
};
