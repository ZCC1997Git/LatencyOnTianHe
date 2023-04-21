#include<compiler/m3000.h>
#include<hthread_device.h>
#include <sys/time.h>

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
	struct Node* next;
	char paddings[CACHELINE_SIZE-sizeof(struct Node*)];
};


__global__ void latency_ddr(int iterations,int count,struct Node* p)
{
    hthread_printf("%d %d\n",iterations,count);

    struct Node* node=&p[0];
    while(iterations--)
    {
        for(int i=0;i<count;i++)
        {
            N1024(node);
        }
    }
    hthread_printf("%ld\n",node);
}

__global__ void latency_hbm(int iterations,int count,int len)
{
    hthread_printf("%d %d %d\n",iterations,count,len);
    
    struct Node* p=hbm_malloc(len*sizeof(struct Node));

    for(int i=0;i<len-1;i++)
        p[i].next=&(p[i+1]);
    p[len-1].next=&(p[0]);

    struct Node* node=&p[0];

    while(iterations--)
    {
        for(int i=0;i<count;i++)
        {
            N1024(node);
        }
    }

    hthread_printf("%ld\n",node);
    hbm_free(p);
}

#define GSM (4*1024*1024)
#define LEN_gsm (GSM/CACHELINE_SIZE)
__share__ struct Node p[LEN_gsm];

__global__ void latency_GSM(int iterations)
{

    hthread_printf("%d \n",iterations);
    int len=LEN_gsm;
    int count=(len+1024-1)/1024;
   

    for(int i=0;i<len-1;i++)
        p[i].next=&(p[i+1]);
    p[len-1].next=&(p[0]);

    struct Node* node=&p[0];

    while(iterations--)
    {
        for(int i=0;i<count;i++)
        {
            N1024(node);
        }
    }

    hthread_printf("%ld\n",node);
}

#define SCALER (50*1024)
#define LEN_scaler (SCALER/CACHELINE_SIZE)
__global__ void latency_scaler(int iterations)
{

    hthread_printf("%d \n",iterations);
    int len=LEN_scaler;
    int count=(len+1024-1)/1024;

    struct Node* p=scalar_malloc(SCALER);

    for(int i=0;i<len-1;i++)
        p[i].next=&(p[i+1]);
    p[len-1].next=&(p[0]);

    struct Node* node=&p[0];

    while(iterations--)
    {
        for(int i=0;i<count;i++)
        {
            N1024(node);
        }
    }

    hthread_printf("%ld\n",node);
    scalar_free(p);
}

#define VECTOR (700*1024)
__global__ void latency_vector(int iterations)
{
    // double data[16];
    // for(int i=0;i<16;i++)
    //     data[i]=2;
    volatile lvector double data_ref_sum;
    register lvector double data_ref;
    // vector_load(data,&data_ref,16*sizeof(double));

    lvector double*p=vector_malloc(700*1024);
    int len=VECTOR/sizeof(double)/16;

    hthread_printf("%d %d\n",iterations,len);

    for(int i=0;i<len;i++)
        p[i]=data_ref;
    
    while(iterations--)
    {
        for(int i=0;i<len;i++)
            data_ref+=p[i];
        
        data_ref_sum=data_ref;
    }
    
    vector_free(p);
}


