#include "udpServer.hpp"
//采用./server 127.0.0.1（[ip]） 8080([port])的形式传入IP地址
void Usage(string proc)
{
    cout<<proc<<"IP"<<" "<<"PORT"<<endl;
}
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);//程序名称server
    }
    Server* sp=new Server(argv[1],atoi(argv[2]));//IP地址和端口号构造,字符串转成整形
    sp->InitServer();
    sp->Start();
    delete sp;
}
