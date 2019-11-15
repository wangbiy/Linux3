#include "protocol.hpp"
int main()
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        cerr<<"socket error"<<endl;
        exit(1);
    }
    struct sockaddr_in peer;
    peer.sin_family=AF_INET;
    peer.sin_port=htons(8888);
    peer.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(sock,(struct sockaddr*)&peer,sizeof(peer))<0)
    {
        cerr<<"connect error"<<endl;
        exit(2);
    }
    for(;;)
    {
        Request rq;
        Response rsp;
        cout<<"Please Enter Your op<+-*/%,q[quit]:q# ";
        cin>>rq.op;
        if(rq.op=='q')
            break;
        cout<<"Please Enter Your Data<x,y># ";
        cin>>rq.x>>rq.y;
        send(sock,&rq,sizeof(rq),0);
        recv(sock,&rsp,sizeof(rsp),0);
        cout<<"status: "<<rsp.status<<endl;
        cout<<"result: "<<rsp.result<<endl;
    }
    close(sock);
}
