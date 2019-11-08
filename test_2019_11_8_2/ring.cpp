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
* FileName:ring.cpp
* Author  :Daisy
* Created :2019-11-08
* Purpose :
* ==============================================================================
*/
#include<iostream>
#include <vector>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
using namespace std;
class RingQueue
{
    private:
        vector<int> buf;
        int cap;
        sem_t sem_blank;
        sem_t sem_data;
        int c_step;
        int p_step;
    private:
        void P(sem_t& s)
        {
            sem_wait(&s);
        }
        void V(sem_t& s)
        {
            sem_post(&s);
        }
    public:
        RingQueue(int _cap=1024)
            :cap(_cap)
             ,buf(_cap)
        {
            c_step=p_step=0;
            sem_init(&sem_blank,0,cap);
            sem_init(&sem_data,0,0);
        }
        ~RingQueue()
        {
            sem_destroy(&sem_blank);
            sem_destroy(&sem_data);
        }
        //生产者
        void PushData(const int& data)
        {
            P(sem_blank);
            buf[p_step]=data;
            p_step++;
            p_step%=cap;
            V(sem_data);
        }
        //消费者
        void PopData(int& data)
        {
            P(sem_data);
            data=buf[c_step];
            c_step++;
            c_step%=cap;
            V(sem_blank);
        }
};
void* consumerRun(void* arg)
{
    RingQueue* rq=(RingQueue*)arg;
    int data;
    while(1)
    {
        rq->PopData(data);
        cout<<"consum data is :"<<data<<endl;
        sleep(data%5+1);
    }
}
void* producterRun(void* arg)
{
    RingQueue* rq=(RingQueue*)arg;
    while(1)
    {
        int data=rand()%100+1;
        rq->PushData(data);
        cout<<"product data is :"<<data<<endl;
    }
}
int main()
{
    srand((unsigned long)time(NULL));
    RingQueue* rq=new RingQueue(8);
    pthread_t c,p;
    pthread_create(&c,NULL,consumerRun,(void*)rq);
    pthread_create(&p,NULL,producterRun,(void*)rq);

    pthread_join(c,NULL);
    pthread_join(p,NULL);
    delete rq;
}
