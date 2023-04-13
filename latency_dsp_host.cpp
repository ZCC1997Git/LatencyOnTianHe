#include<iostream>
#include<hthread_host.h>
#include<chrono>
using namespace std;
using namespace std::chrono;

#define CACHELINE_SIZE 64
/*the data type*/
struct Node
{
	Node* next;
	char paddings[CACHELINE_SIZE-sizeof(Node*)];
};

void DSP_DDR(int iteration);
void DSP_HBM(int iteration);
void DSP_GSM();
void DSP_scaler();
void DSP_vector();

int main()
{
	/*the iteration number*/
    constexpr int iterations=200;

	// DSP_HBM(iterations);
	// DSP_GSM();
	// DSP_scaler();
	DSP_vector();
	return 0;
}

void DSP_DDR(int iteration)
{
	/*the memory required (in byte)*/
	int Memory=128*1024*1024;

	/*the number of element*/  
    int len=Memory/CACHELINE_SIZE;

	int count=(len+1024-1)/1024;

	int cluster=0;
	int num_thread=1;

	/*open the dsp cluster*/
	hthread_dev_open(cluster);
	hthread_dat_load(cluster,"latency_dsp_device.dat");

	auto p=reinterpret_cast<Node*>(hthread_malloc(cluster,len*sizeof(Node),HT_MEM_RO));
		
	for(int i=0;i<len-1;i++)
			p[i].next=&(p[i+1]);
	p[len-1].next=&(p[0]);

	/*create the thread group*/
	auto thread_group=hthread_group_create(cluster,num_thread,NULL,0,0,NULL);

	/*prepare*/
	unsigned long args[3];
	args[0]=iteration;
	args[1]=count;
	args[2]=(unsigned long)p;

	auto m_start = system_clock::now();
	hthread_group_exec(thread_group,"latency_ddr",2,1,args);
	hthread_group_wait(thread_group);
	auto diff = system_clock::now() - m_start;
    auto t=(unsigned)(duration_cast<milliseconds>(diff).count());

	cout<<t<<endl;
	cout<<"The DDR latency is "<<(double)t/iteration/count/1024*1.0e6<<endl;

	hthread_free(p);
	hthread_dev_close(cluster);
}

void DSP_HBM(int iteration)
{
	/*the memory required (in byte)*/
	int Memory=12*1024*1024;

	/*the number of element*/  
    int len=Memory/CACHELINE_SIZE;

	int count=(len+1024-1)/1024;

	int cluster=0;
	int num_thread=1;

	/*open the dsp cluster*/
	hthread_dev_open(cluster);
	hthread_dat_load(cluster,"latency_dsp_device.dat");

	/*create the thread group*/
	auto thread_group=hthread_group_create(cluster,num_thread,NULL,0,0,NULL);

	/*prepare*/
	unsigned long args[3];
	args[0]=iteration;
	args[1]=count;
	args[2]=len;

	auto m_start = system_clock::now();
	hthread_group_exec(thread_group,"latency_hbm",3,0,args);
	hthread_group_wait(thread_group);
	auto diff = system_clock::now() - m_start;
    auto t=(unsigned)(duration_cast<milliseconds>(diff).count());
	cout<<t<<endl;
	cout<<"The HBM latency is "<<(double)t/iteration/count/1024*1.0e6<<endl;

	hthread_dev_close(cluster);
}


void DSP_GSM()
{
	constexpr int iteration=10000;
	/*the memory required (in byte)*/
	int Memory=4*1024*1024;

	/*the number of element*/  
    int len=Memory/CACHELINE_SIZE;

	int count=(len+1024-1)/1024;

	int cluster=0;
	int num_thread=1;

	/*open the dsp cluster*/
	hthread_dev_open(cluster);
	hthread_dat_load(cluster,"latency_dsp_device.dat");

	/*create the thread group*/
	auto thread_group=hthread_group_create(cluster,num_thread,NULL,0,0,NULL);

	/*prepare*/
	unsigned long args[1];
	args[0]=iteration;

	auto m_start = system_clock::now();
	hthread_group_exec(thread_group,"latency_GSM",1,0,args);
	hthread_group_wait(thread_group);
	auto diff = system_clock::now() - m_start;
    auto t=(unsigned)(duration_cast<milliseconds>(diff).count());
	cout<<t<<endl;
	cout<<"The GSM latency is "<<(double)t/iteration/count/1024*1.0e6<<endl;

	hthread_dev_close(cluster);
}


void DSP_scaler()
{
	constexpr int iteration=100000;
	/*the memory required (in byte)*/
	int Memory=50*1024;

	/*the number of element*/  
    int len=Memory/CACHELINE_SIZE;

	int count=(len+1024-1)/1024;

	int cluster=0;
	int num_thread=1;

	/*open the dsp cluster*/
	hthread_dev_open(cluster);
	hthread_dat_load(cluster,"latency_dsp_device.dat");

	/*create the thread group*/
	auto thread_group=hthread_group_create(cluster,num_thread,NULL,0,0,NULL);

	/*prepare*/
	unsigned long args[1];
	args[0]=iteration;

	auto m_start = system_clock::now();
	hthread_group_exec(thread_group,"latency_scaler",1,0,args);
	hthread_group_wait(thread_group);
	auto diff = system_clock::now() - m_start;
    auto t=(unsigned)(duration_cast<milliseconds>(diff).count());
	cout<<t<<endl;
	cout<<"The Scaler latency is "<<(double)t/iteration/count/1024*1.0e6<<endl;

	hthread_dev_close(cluster);
}

void DSP_vector()
{
	constexpr int iteration=10000;

	int cluster=0;
	int num_thread=1;

	/*open the dsp cluster*/
	hthread_dev_open(cluster);
	hthread_dat_load(cluster,"latency_dsp_device.dat");

	/*create the thread group*/
	auto thread_group=hthread_group_create(cluster,num_thread,NULL,0,0,NULL);

	/*prepare*/
	unsigned long args[1];
	args[0]=iteration;

	auto m_start = system_clock::now();
	hthread_group_exec(thread_group,"latency_vector",1,0,args);
	hthread_group_wait(thread_group);
	auto diff = system_clock::now() - m_start;
    auto t=(unsigned)(duration_cast<milliseconds>(diff).count());
	cout<<t<<endl;

	hthread_dev_close(cluster);
}
