#include <stdio.h>
#include <sys/select.h>
int main()
{
    printf("%d\n",sizeof(fd_set)*8);
}
