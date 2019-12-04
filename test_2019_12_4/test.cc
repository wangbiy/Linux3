#include <iostream>
#include <string>
#include <sstream>
using namespace std;
int main()
{
    string s="content-length: 1234\na:b\nc:d\nefg:321\n";
    //size_t pos=s.find_first_of("\n");//这个接口的返回值是XX的下标
    size_t pos=s.find("\n",0);
    if(string::npos==pos)
        return 1;
    cout<<pos<<endl;
    string sub=s.substr(0,pos);
    cout<<sub<<endl;

    pos=s.find("\n",pos+1);
    cout<<pos<<endl;
}
