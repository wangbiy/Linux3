#include <iostream>
#include <poll.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
int main()

{
    struct pollfd rfds;
    rfds.fd=0;
    rfds.events=POLLIN;//读事件
    rfds.events|=POLLOUT;//使用按位或加上写事件
    rfds.revents=0;
    string buf;
    while(1)
    {
        switch(poll(&rfds,1,0))
        {
            case 0:
                cout<<"time out"<<endl;
                break;
            case -1:
                cout<<"poll error"<<endl;
                break;
            default://事件就绪
                {
                    if(rfds.revents & POLLIN)//返回时检测
                    {
                        cin>>buf;
                    }
                    if(rfds.revents & POLLOUT)//判断写事件就绪
                    {
                        cout<<"echo# "<<endl;
                    }
                    sleep(1);
                }
                break;
        }
    }
    system("pause");
    return 0;
}
