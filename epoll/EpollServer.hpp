#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <cstring>
using namespace std;
class Server
{
    private:
        int port;
        int lsock;
        int epfd;
    public:
        Server(int _port=8080)
            :port(_port)
             ,lsock(-1)
             ,epfd(-1)
        {}
        void InitServer()
        {
            lsock=socket(AF_INET,SOCK_STREAM,0);
            if(lsock<0)
            {
                cerr<<"socket error"<<endl;
                exit(2);
            }
            struct sockaddr_in local;
            local.sin_family=AF_INET;
            local.sin_addr.s_addr=htonl(INADDR_ANY);
            local.sin_port=htons(port);
            if(bind(lsock,(struct sockaddr*)&local,sizeof(local))<0)
            {
                cerr<<"bind error"<<endl;
                exit(3);
            }
            if(listen(lsock,5)<0)
            {
                cerr<<"listen error"<<endl;
                exit(4);
            }
            int opt=1;
            setsockopt(lsock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
            if((epfd=epoll_create(256))<0)
            {
                cerr<<"epoll_create error"<<endl;
                exit(5);
            }
        }
        void HanderEvents(int epfd,struct epoll_event* revs,int num)
        {
            struct epoll_event ev;
            for(int i=0;i<num;++i)
            {
                int sock=revs[i].data.fd;//拿到套接字
                if(revs[i].events & EPOLLIN)//检测是否有读事件就绪
                {
                    if(sock==lsock)
                    {
                        //link event ready,获取新链接
                        struct sockaddr_in peer;
                        socklen_t len=sizeof(peer);
                        int new_sock=accept(lsock,(struct sockaddr*)&peer,&len);
                        if(new_sock<0)
                        {
                            cerr<<"accept error"<<endl;
                            continue;
                        }
                        cout<<"get a new link,fd: "<<new_sock<<endl;
                        //连接获取成功
                        //将当前文件描述符添加进epoll模型中
                        ev.events=EPOLLIN;
                        ev.data.fd=new_sock;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,new_sock,&ev);
                    }
                    else
                    {
                        //read event ready，普通socket
                        char buf[1024];
                        ssize_t s=recv(sock,buf,sizeof(buf)-1,0);
                        if(s>0)
                        {
                            buf[s]=0;
                            cout<<buf<<endl;
                            ev.events=EPOLLOUT;//改为写
                            ev.data.fd=sock;
                            epoll_ctl(epfd,EPOLL_CTL_MOD,sock,&ev);//读完后想要写，就要将文件描述符改为写
                        }
                        else if(s==0)
                        {
                            cout<<"link .....close"<<endl;
                            close(sock);
                            epoll_ctl(epfd,EPOLL_CTL_DEL,sock,nullptr);
                        }
                        else
                        {
                            cout<<"recv error"<<endl;
                            close(sock);
                            epoll_ctl(epfd,EPOLL_CTL_DEL,sock,nullptr);
                        }
                    }
                }//if
                else if(revs[i].events & EPOLLOUT)//关心写事件是否就绪
                {
                    string http_echo="HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Hello EPOLL Server</h1><img src=\"https://c-ssl.duitang.com/uploads/item/201507/13/20150713153609_YKU8V.thumb.700_0.jpeg\" alt=\"test\"/></body></html>";
                    send(sock,http_echo.c_str(),http_echo.size(),0);
                    close(sock);//短链接
                    epoll_ctl(epfd,EPOLL_CTL_DEL,sock,nullptr);
                }
                else
                {
                    cout<<"bug?"<<endl;
                }
            }//for
        }
        void Run()
        {
            //将lsock注册进epoll模型
            struct epoll_event ev;
            ev.events=EPOLLIN;
            ev.data.fd=lsock;
            epoll_ctl(epfd,EPOLL_CTL_ADD,lsock,&ev);//红黑树中新增了一个节点，叫做lsock
            struct epoll_event revs[128];
            for(;;)
            {
                int timeout=-1;
                int num=0;
                switch((num=epoll_wait(epfd,revs,128,timeout)))
                {
                    case 0:
                        cout<<"time out"<<endl;
                        break;
                    case -1:
                        cout<<"epoll wait error"<<endl;
                        break;
                    default:
                        //目前事件在revs中，只有listen事件
                        HanderEvents(epfd,revs,num);//处理事件
                        break;
                }

            }
        }
        ~Server()
        {
            if(lsock>=0)
            {
                close(lsock);
            }
            if(epfd>=0)
            {
                close(lsock);
            }
        }
};
