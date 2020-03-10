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
#include "ThreadPool.hpp"
using namespace std;
class TcpServer
{
    private:
        int listen_sock;//监听套接字，专门用来获取新链接
        int port;
        string ip;
        ThreadPool* tp;//线程池
    public:
        TcpServer(string _ip="127.0.0.1",int _port=8080)
             :port(_port)
              ,ip(_ip)
              ,tp(NULL)
        {
            listen_sock=-1;
        }
        ~TcpServer()
        {
            if(listen_sock>0)
            {
                close(listen_sock);
            }
            delete tp;//销毁线程池
        }
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
            tp=new ThreadPool();//创建线程池
            tp->InitThreadPool();//初始化线程池
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
                Task t(sock);//将套接字交给线程池
                tp->PushTask(t);//线程池将任务放进去即可
            }
        }
};
void Usage(string proc)
{
    cout<<proc<<"[ip:port:]"<<endl;
}
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    TcpServer* tp=new TcpServer(argv[1],atoi(argv[2]));
    tp->InitServer();
    tp->Start();
    delete tp;
    return 0;
}

