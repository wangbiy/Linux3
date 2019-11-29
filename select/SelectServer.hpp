#pragma once
#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#define SIZE sizeof(fd_set)*8
class SelectServer
{
    private:
        int port;
        int lsock;
    public:
        SelectServer(int _port=8888)
            :port(_port)
            ,lsock(-1)
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
            local.sin_port=htons(port);
            local.sin_addr.s_addr=htonl(INADDR_ANY);
            if(bind(lsock,(struct sockaddr*)&local,sizeof(local))<0)
            {
                cerr<<"binf error"<<endl;
                exit(3);
            }
            if(listen(lsock,5)<0)//5表示底层全连接队列长度
            {
                cerr<<"listen error"<<endl;
                exit(4);
            }
            int opt=1;
            setsockopt(lsock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//解决地址复用的问题
        }
        void Run()
        {
            int fd_array[SIZE];//保存历史上获取的文件描述符
            int i=0;
            for(;i<SIZE;++i)//对数组初始化
            {
                fd_array[i]=-1;
            }
            fd_set rfds;
            fd_array[0]=lsock;//第一个参数被设置成lsock，其他是-1
            int max=lsock;//文件描述符最大值
            for(;;)
            {
                struct sockaddr_in peer;
                socklen_t len=sizeof(peer);
                struct timeval timeout={0,0};//每隔0秒钟timeout,对时间重新设置
                //对读文件描述符集进行初始化
                FD_ZERO(&rfds);
                for(i=0;i<SIZE;++i)
                {
                    if(fd_array[i]==-1)
                    {
                        continue;//不用设置
                    }
                //将合法文件描述符数组添加进读文件描述符集
                    FD_SET(fd_array[i],&rfds);
                    if(fd_array[i]>max)//进行最大的文件描述符值更新
                    {
                        max=fd_array[i];
                    }
                }
//                switch(select(max+1,&rfds,nullptr,nullptr,&timeout))//timeout轮询
                switch(select(max+1,&rfds,nullptr,nullptr,nullptr))//阻塞方式
                {
                    case 0:
                        cout<<"timeout..."<<endl;
                        break;
                    case -1:
                        cerr<<"select error"<<endl;
                        break;
                    default:
                        for(i=0;i<SIZE;++i)
                        {
                            if(fd_array[i]==-1)
                            {
                                continue;
                            }
                            if(FD_ISSET(fd_array[i],&rfds))//判断读文件描述符是否设置成功
                            {
                                if(fd_array[i]==lsock)//如果是lsock,才能进行accept，才一定不会被阻塞
                                {
                                    int fd=accept(lsock,(struct sockaddr*)&peer,&len);
                                    if(fd<0)
                                    {
                                        cerr<<"accept error"<<endl;
                                        continue;
                                    }
                                    cout<<"get a new link..."<<endl;//获得新链接
                                    //拿到的文件描述符不能直接读写，一但直接读写有可能会因为数据未就位而阻塞
                                    //将新获得的文件描述符添加进数组中，
                                    //下一次就可以将新文件描述符设置进读文件描述符集
                                    int j=1;
                                    for(;j<SIZE;++j)
                                    {
                                        if(fd_array[j]==-1)
                                        {
                                            break;
                                        }
                                    }
                                    if(j==SIZE)//数组被放满
                                    {
                                        cout<<"fd array is full!"<<endl;
                                        close(fd);
                                    }
                                    else
                                    {
                                        fd_array[j]=fd;
                                    }
                                }
                                else//普通文件描述符
                                {
                                    char buf[1024];
                                    ssize_t s=recv(fd_array[i],buf,sizeof(buf),0);
                                    if(s>0)
                                    {
                                        buf[s]=0;
                                        cout<<"client# "<<buf<<endl;
                                        send(fd_array[i],buf,strlen(buf),0);
                                    }
                                    else if(s==0)
                                    {
                                        cout<<"client quit!"<<endl;
                                        close(fd_array[i]);//将连接关掉
                                        fd_array[i]=-1;//回收这个文件描述符
                                    }
                                    else
                                    {
                                        cerr<<"recv error"<<endl;
                                        close(fd_array[i]);
                                        fd_array[i]=-1;
                                    }
                                }
                            }
                        }
                        break;
                }
            }
        }
        ~SelectServer()
        {
            if(lsock>=0)
            {
                close(lsock);
            }
        }
};
