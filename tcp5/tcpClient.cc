#include <iostream>
using namespace std;
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
class TcpClient
{
    private:
        int sock;
        string server_ip;
        int server_port;
    public:
        TcpClient(string _ip ="127.0.0.1",int _port=8080)
            :server_ip(_ip)
             ,server_port(_port)
        {
            sock=-1;
        }
        void InitClient()
        {
            sock=socket(AF_INET,SOCK_STREAM,0);
            if(sock<0)
            {
                cerr<<"sock socket error"<<endl;
                exit(2);
            }
            cout<<"sock socket success"<<sock<<endl;
        }
        void Run()
        {
            //链接
            struct sockaddr_in server;
            server.sin_family=AF_INET;
            server.sin_addr.s_addr=inet_addr(server_ip.c_str());
            server.sin_port=htons(server_port);
            if(connect(sock,(struct sockaddr*)&server,sizeof(server))<0)
            {
                cerr<<"connect error"<<endl;
                return;
            }
            //链接成功
            //开始发送消息
            string message;
            char buf[1024];
            while(1)
            {
                cout<<"Please Enter# ";
                cin>>message;
                write(sock,message.c_str(),message.size());
                ssize_t s=read(sock,buf,sizeof(buf)-1);
                if(s>0)
                {
                    buf[s]=0;
                    cout<<"server echo# "<<buf<<endl;
                }
            }
        }
        ~TcpClient()
        {
            if(sock>0)
            {
                close(sock);
            }
        }
};
void Usage(string proc)
{
    cout<<proc<<"[ip port:]"<<endl;
}
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    TcpClient* tp=new TcpClient(argv[1],atoi(argv[2]));
    tp->InitClient();
    tp->Run();
    delete tp;
    return 0;
}
