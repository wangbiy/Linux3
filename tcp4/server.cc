#include "tcpServer.hpp"
void Usage(string proc)
{
    cout<<proc<<"port"<<endl;
}
int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        exit(1);
    }
    int port=atoi(argv[1]);
    //不需要ip地址
    TcpServer* tsp=new TcpServer(port);
    tsp->InitServer();
    tsp->Start();
    delete tsp;
}
