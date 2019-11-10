#include "udpServer.hpp"
//server 127.0.0.1 8080
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
