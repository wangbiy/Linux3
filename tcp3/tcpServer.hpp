#pragma once
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <pthread.h>
using namespace std;
class TcpServer
{
    private:
        int listen_sock;//监听套接字，专门用来获取新链接
        string ip;
        int port;
    public:
        TcpServer(string _ip="127.0.0.1",int _port=8080)
            :ip(_ip)
             ,port(_port)
        {
            listen_sock=-1;
        }
        ~TcpServer()
        {}
        void InitServer()
        {
            listen_sock=socket(AF_INET,SOCK_STREAM,0);//ipv4,tcp协议，因此使用listen_sock_STREAM
            if(listen_sock<0)
            {
                cerr<<"listen_socket error"<<endl;
                exit(2);
            }
            cout<<"socket create success: "<<listen_sock<<endl;
            //绑定
            struct sockaddr_in local;
            local.sin_family=AF_INET;
            local.sin_addr.s_addr=inet_addr(ip.c_str());
            local.sin_port=htons(port);

            if(bind(listen_sock,(struct sockaddr*)&local,sizeof(local))<0)
            {
                cerr<<"bind error"<<endl;
                exit(3);
            }
            cout<<"bind success: "<<listen_sock<<endl;
            //设置为监听状态
            if(listen(listen_sock,5)<0)
            {
                cerr<<"listen error"<<endl;
                exit(4);
            }
        }
        static void ServiceIO(int sock)
        {
            char buf[1024];
            while(1)
            {
                //进行收发消息
                ssize_t s=read(sock,buf,sizeof(buf)-1);
                if(s>0)
                {
                    buf[s]=0;
                    cout<<"client# "<<buf<<endl;
                    write(sock,buf,strlen(buf));
                }
                else if(s==0)
                {
                    cout<<"client quit"<<endl;//客户端退出
                    break;
                }
                else
                    cerr<<"read error"<<endl;
            }
        }
        static void* ThreadRun(void* arg)//类内的成员函数，因为是带this指针的，不负好pthread_create参数中线程处理函数回调机制，因此要设置为static的
        {
            pthread_detach(pthread_self());//将新线程分离，因为如果新线程退出，主线程不采用pthread_join的方式进行等待，就会产生类似于僵尸进程的问题，因此要将线程分离
            int sock =*(int*) arg;//涉及C++11中的四种类型转换
            ServiceIO(sock);
            delete (int*)arg;
            //所有的线程是共享文件描述符的，因此不能关闭文件描述符，这也是与多进程之间的不同，但是多线程可以在服务完成之后关闭文件描述符
            close(sock);

        }
        void Start()
        {
            struct sockaddr_in peer;//对端
            while(true)
            {
                //先接收连接
                socklen_t len=sizeof(peer);
                int sock=accept(listen_sock,(struct sockaddr*)&peer,&len);//这里的sock表示新的文件描述符，是用于与客户端通信的，与监听套接字有区别
                if(sock<0)
                {
                    cerr<<"accept error!"<<endl;
                    continue;//并不是退出，并没有什么影响
                }
                cout<<"get a new client[IP:PORT]"<<inet_ntoa(peer.sin_addr)<<":"<<ntohs(peer.sin_port)<<endl;//拿到客户端的IP地址，以点分十进制的方式打印，因此要用到地址转换函数将4字节的字符串转换为点分十进制的ip地址，使用inet_ntoa接口，而打印port号要将网络号转换为主机号，使用ntohs接口
                pthread_t tid;
                int* p=new int(sock);//由于sock是int类型的，我们在实现ThreadRun函数时无法直接将void*转换为int，因此用p指向sock，然后将p作为ThreadRun的参数传进去即可
                pthread_create(&tid,NULL,ThreadRun,(void*)p);//创建新线程
            }
        }
};

