#include"tcpClient.hpp"
void Usage(string proc)
{
    cout<<proc<<"server_ip"<<"server_port"<<endl;
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
