#include<iostream>
#include<sys/time.h>
#include<chrono>
#include<sched.h>
using namespace std::chrono;
using namespace std;

#define N1(node) node=node->next;
#define N2(node) N1(node);N1(node);
#define N4(node) N2(node);N2(node);
#define N8(node) N4(node);N4(node);
#define N16(node) N8(node);N8(node);
#define N32(node) N16(node);N16(node);
#define N64(node) N32(node);N32(node);
#define N128(node) N64(node);N64(node);
#define N256(node) N128(node);N128(node);
#define N512(node) N256(node);N256(node);
#define N1024(node) N512(node);N512(node);

#define CACHELINE_SIZE 64
/*the data type*/
struct Node 
{
    Node* next;
    char paddings[CACHELINE_SIZE-sizeof(Node*)];
};

/*the number of byte used in stride manner(the size of cache line)*/

void stride_initialize(Node* p,int array_size);
unsigned latecy_check(Node* p,int iteration,int count);

int main()
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(0, sizeof(set), &set);

    /*the iteration number*/
    constexpr int iterations=10000;

    /*the size in byte*/
    /*L1: 256 k*/
    /*L2: 4 M*/
    /*L3: 16 M*/
    constexpr int Memory=6*512*1024;

    /*the number of element*/  
    constexpr int len=Memory/CACHELINE_SIZE;

    Node* p=new Node[len];

    stride_initialize(p,len);
    
    int count=(len+1024-1)/1024;

    auto t=latecy_check(p,iterations,count);

    cout<<"Latency for "<< (double)Memory/1024/1024<<" Mib is\t"<<
        (double)t/count/1024/iterations*1000000<<"  ns"<<endl;
    delete[] p;

    return 0;
}


void stride_initialize(Node* p,int array_size)
{
    for(int i=0;i<array_size-1;i++)
        p[i].next=&(p[i+1]);
    p[array_size-1].next=&(p[0]);
}

unsigned latecy_check(Node* p,int iteration,int count)
{
    Node* node=&p[0];

    auto m_start = system_clock::now();
    while(iteration--)
    {
        for(int i=0;i<count;i++)
        {
            N1024(node);
        }
    }

    auto m_end = system_clock::now();
    auto diff = system_clock::now() - m_start;
    auto t=(unsigned)(duration_cast<milliseconds>(diff).count());
    cout<<node<<endl;
    return t;
}



