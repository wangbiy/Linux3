#pragma once
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
using namespace std;
struct Request
{
    int x;
    int y;
    char op;//'+''-''*''%''/'
};
struct Response
{
    int result;
    int status;//0--->success 非0---->fail

};
