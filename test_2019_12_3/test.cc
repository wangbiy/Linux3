#include <iostream>
#include <string>
#include <sstream>
using namespace std;
int main()
{
    string s="GET    /index.html     http/1.1";
    stringstream ss(s);
    string method;
    string uri;
    string version;
    ss>>method>>uri>>version;
    cout<<"method: "<<method<<endl;
    cout<<"uri: "<<uri<<endl;
    cout<<"version: "<<version<<endl;
}
