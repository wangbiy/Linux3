#include "protocol.hpp"
void* Routine(void* arg)
{
    pthread_detach(pthread_self());
    int sock=*(int*)arg;
    delete (int*)arg;
    Request rq;
    while(1)
    {
        ssize_t s=recv(sock,&rq,sizeof(rq),0);
        if(s>0)
        {
            Response rsp={0,0};
            switch(rq.op)
            {
                case '+':
                    rsp.result=rq.x+rq.y;
                    break;
                case '-':
                    rsp.result=rq.x-rq.y;
                    break;
                case '*':
                    rsp.result=rq.x*rq.y;
                    break;
                case '/':
                    if(0==rq.y)
                    {
                        rsp.status=1;
                    }
                    else
                        rsp.result=rq.x/rq.y;
                    break;
                case '%':
                    if(0==rq.y)
                    {
                        rsp.status=2;
                    }
                    else
                        rsp.result=rq.x%rq.y;
                    break;
                default:
                    rsp.status=3;
                    break;
            }
            send(sock,&rsp,sizeof(rsp),0);
        }
        else if(s==0)//对端将链接关闭
        {
            cout<<"Client....quit"<<endl;
            break;
        }
        else
        {
            break;
        }
    }
     close(sock);//触发四次挥手,这个位置很重要，必须在循环结束之后关掉文件描述符，否则再进行一次循环时，读取文件描述符会出错
}
int main()
{
    int listen_sock=socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock<0)
    {
        cerr<<"socket error"<<endl;
        exit(1);
    }
    struct sockaddr_in local;
    local.sin_family=AF_INET;
    local.sin_port=htons(8888);
    local.sin_addr.s_addr=htonl(INADDR_ANY);//0
    if(bind(listen_sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
        cerr<<"bind error"<<endl;
        exit(2);
    }
    if(listen(listen_sock,5)<0)
    {
        cerr<<"listen error"<<endl;
        exit(3);
    }
    struct sockaddr_in peer;
    for(;;)
    {
        socklen_t len=sizeof(peer);//一定要重新计算
        int sock=accept(listen_sock,(struct sockaddr*)&peer,&len);
        if(sock<0)
        {
            cerr<<"accept error"<<endl;
            continue;
        }
        cout<<"get a new link"<<endl;
        pthread_t id;
        int* p=new int(sock);
        pthread_create(&id,nullptr,Routine,(void*)p);
    }

}
