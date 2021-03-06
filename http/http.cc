#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;
void* Routine(void* arg)
{
    int* p=(int*)arg;
    int sock=*p;
    delete p;
    char request[10240];
    ssize_t s=recv(sock,request,sizeof(request)-1,0);
    if(s>0)
    {
        request[s]=0;
        cout<<"----------------------------"<<endl;//证明这个是一个请求
        cout<<request<<endl;//读完
        cout<<"----------------------------"<<endl;

        //string response = "HTTP/1.0 200 OK\r\n<html><h1>hello http server</h1></html>";
       // string response = "HTTP/1.0 302 Found\r\nLocation:https:// www.baidu.com\r\n\r\n";//使用临时重定向状态码302来响应，最后再加一个/r/n表示空行
       // string response = "HTTP/1.0 301 Moved Permanently\r\nLocation:https:// www.baidu.com\r\n\r\n";//使用永久重定向状态码301来响应，最后再加一个/r/n表示空行
       // string response = "HTTP/1.0 404 Not Found\r\n";//使用404来响应,表示服务器无法处理请求，即客户端出现错误
       string response="HTTP/1.0 200 OK\r\nSet-Cookie: a=1234;\r\n\r\n<html><h1>OK!</h1></html>";//测试cookie
        send(sock,response.c_str(),response.size(),0);
    }
    else if(s==0)
    {
        cout<<"client quit"<<endl;
    }
    else
    {
        cout<<"recv error"<<endl;
    }
    close(sock);
}
int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        cout<<"Usage : "<<argv[0]<<"port"<<endl;
        return 1;
    }
    int port=atoi(argv[1]);
    int listen_sock=socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock<0)
    {
        cerr<<"socket error"<<endl;
        return 2;
    }
    struct sockaddr_in local;
    local.sin_family=AF_INET;
    local.sin_port=htons(port);
    local.sin_addr.s_addr=htonl(INADDR_ANY);//随机绑定本主机Ip
    if(bind(listen_sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
        cerr<<"bind error"<<endl;
        return 3;
        
    }
    if(listen(listen_sock,5)<0)
    {
        cerr<<"listen error"<<endl;
        return 4;
    }
    for(;;)
    {
        struct sockaddr_in peer;
        socklen_t len=sizeof(peer);
        int sock=accept(listen_sock,(struct sockaddr*)&peer,&len);
        if(sock<0)
        {
            cerr<<"accept error"<<endl;
            continue;
        }
        cout<<"get a new client: "<<sock<<endl;
        int* fd=new int(sock);
        pthread_t tid;
        pthread_create(&tid,nullptr,Routine,fd);
    }

}
