#pragma once
#include <queue>
#include <iostream>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <map>
using namespace std;
map<string,string> dict= {
    {"apple","苹果"},
    {"banana","香蕉"}
};
class Task//我们设定这个任务是进行单词的翻译
{
    private:
        int fd;//需要将翻译结果给客户端
    public:
        Task()
        {}
        Task(int _fd)
             :fd(_fd)
        {}
        ~Task()
        {}
        void Run()
        {
            string key;
            char buf[1024];
            ssize_t s=recv(fd,buf,sizeof(buf)-1,0);
            if(s>0)
            {
                buf[s]=0;
                key=buf;
            }
            string value;
            auto it=dict.find(key);
            if(it!=dict.end())
            {
                value=it->second;
            }
            else
                value="unknow";
            send(fd,value.c_str(),value.size(),0);//将翻译结果发送到文件描述符fd
            close(fd);
        }

};
class ThreadPool
{
    private:
        int num;
        queue<Task> q;
        pthread_mutex_t lock;//加锁
        pthread_cond_t cond;//条件变量
    public:
        ThreadPool(int _num=5)//默认创建5个线程
            :num(_num)
        {
            pthread_mutex_init(&lock,NULL);
            pthread_cond_init(&cond,NULL);
        }
        void LockQueue()
        {
            pthread_mutex_lock(&lock);
        }
        void UnlockQueue()
        {
            pthread_mutex_unlock(&lock);
        }
        bool IsEmpty()
        {
            return (q.size()==0 ? true:false);
        }
        void ThreadWait()
        {
            pthread_cond_wait(&cond,&lock);//必须将占有的锁释放掉，因为在临界资源中等待，并且将锁释放
        }
        void ThreadSignal()
        {
            pthread_cond_signal(&cond);
        }
        void PushTask(Task &t)
        {
            for(;;)
            {
                LockQueue();
                q.push(t);
                UnlockQueue();
                //唤醒线程来消费任务
                ThreadSignal();
            }
        }
        Task PopTask()
        {
            Task t=q.front();
            q.pop();
            return t;
        }
        static void* Routine(void* arg)
        {
            ThreadPool* self=(ThreadPool*)arg;
            pthread_detach(pthread_self());//线程分离
            for(;;)
            {
                self->LockQueue();
                while(self->IsEmpty())//如果出现异常，还要再检测一次，保证往下走队列不为空
                {
                    self->ThreadWait();
                }
                //有任务
                Task t=self->PopTask();
                self->UnlockQueue();//解锁，因为下面任务处理与队列无关
                t.Run();//处理 任务与队列没有关系
            }
        }
        void InitThreadPool()
        {
            pthread_t t;
            for(auto i=0;i<num;++i)
            {
                pthread_create(&t,NULL,Routine,(void*)this);//将当前对象传进去
            }
        }
        ~ThreadPool()
        {
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);
        }

};
