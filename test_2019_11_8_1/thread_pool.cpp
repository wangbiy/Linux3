/*
               #########                       
               #########                       
              ############                     
              #############                    
             ##  ###########                   
            ###  ###### #####                  
            ### #######   ####                 
           ###  ########## ####                
          ####  ########### ####               
        ####   ###########  #####              
       #####   ### ########   #####            
      #####   ###   ########   ######          
     ######   ###  ###########   ######        
    ######   #### ##############  ######       
   #######  #####################  ######      
   #######  ######################  ######     
  #######  ###### #################  ######    
  #######  ###### ###### #########   ######    
  #######    ##  ######   ######     ######    
  #######        ######    #####     #####     
   ######        #####     #####     ####      
    #####        ####      #####     ###       
     #####       ###        ###      #         
       ###       ###        ###                
        ##       ###        ###                
_________#_______####_______####______________ 
               我们的未来没有BUG               
* ==============================================================================
* FileName:thread_pool.cpp
* Author  :Daisy
* Created :2019-11-08
* Purpose :
* ==============================================================================
*/
#include<iostream>
#include <queue>
using namespace std;
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
class Task
{
    private:
        int x;
        int y;
    public:
        Task() 
        {}
        ~Task()
        {}
        void SetData(int _x,int _y)
        {
            x=_x;
            y=_y;
        }
        int Run()
        {
            return x+y;
        }
};
class ThreadPool
{
    private:
        int num;//表示有多少线程
        queue<Task> q;//任务队列
        pthread_mutex_t lock;
        pthread_cond_t cond;
    public:
        void LockQueue()
        {
            pthread_mutex_lock(&lock);
        }
        void UnlockQueue()
        {
            pthread_mutex_unlock(&lock);
        }
        bool QueueIsEmpty()
        {
            return (q.size()==0 ? true:false);
        }
        void ThreadWait()
        {
            pthread_cond_wait(&cond,&lock);
        }
        void PopTask(Task& t)
        {
            t=q.front();
            q.pop();
        }
        void PushTask(Task& t)
        {
            q.push(t);
        }
        void NotifyThread()
        {
            pthread_cond_signal(&cond);
        }
    public:
        ThreadPool(int _num=6)
            :num(_num)
        {
            pthread_mutex_init(&lock,NULL);
            pthread_cond_init(&cond,NULL);
        }
        ~ThreadPool()
        {
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);
        }
        static void* HandlerTask(void* arg)//成员函数,在类内使用多线程需要将线程处理函数设置为static的
        {
            pthread_detach(pthread_self());//将当前线程分离
            ThreadPool *tp=(ThreadPool*)arg;
            for(;;)
            {
               tp->LockQueue();
                if(tp->QueueIsEmpty())//如果队列为空，线程等待有任务到来
                {
                    tp->ThreadWait();
                }
                Task t;
                tp->PopTask(t);//如果任务不为空，拿任务
                tp->UnlockQueue();
                //任务拿出来后开始处理任务
                int result=t.Run();
                cout<<"Thread :"<<pthread_self()<<",result :"<<result<<endl;
            }
        }
        void InitThreadPool()
        {
            int i=0;
            pthread_t tid;
            for(;i<num;++i)
            {
                pthread_create(&tid,NULL,HandlerTask,(void*)this);//创建num个线程
            }
        }
        void AddTask(Task& t)
        {
            LockQueue();
            PushTask(t);//将任务t放进来
            UnlockQueue();
            NotifyThread();//通知线程
        }
        
};

int main()
{
    ThreadPool *tp=new ThreadPool(5);
    tp->InitThreadPool();
    int count=0;
    for(;;)
    {
        int x=count%1000+100;
        int y=count%2000+300;
        count++;
        Task t;
        t.SetData(x,y);
        tp->AddTask(t);
        sleep(1);
    }
    delete tp;
}
