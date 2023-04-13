ENV_ROOT = ${PROG_ENV}/hthreads
DSP_ROOT=${PROG_ENV}/dsp_compiler

CFLAGS=--std=c11 -Wall -c -O3 -fenable-m3000 -ffunction-sections -flax-vector-conversions -I./ -I$(ENV_ROOT)/include -I$(DSP_ROOT)/include/
LDFLAGS= -L$(ENV_ROOT)/lib --gc-sections -Tdsp.lds


ALL:Latency_MT latency_dsp

latency_dsp: latency_dsp_host.cpp  latency_dsp_device.dat
	g++ -O3 -march=native -std=c++17  -I $(ENV_ROOT)/include  -fopenmp  -Wall latency_dsp_host.cpp  -o latency_dsp $(ENV_ROOT)/lib/libhthread_host.a $(ENV_ROOT)/lib/libhthread_device.a -lpthread -fopenmp

latency_dsp_device.dat: latency_dsp_device.out
	$(DSP_ROOT)/bin/MT-3000-makedat -J latency_dsp_device.out
	
latency_dsp_device.out: latency_dsp_device.o
	$(DSP_ROOT)/bin/MT-3000-ld $(LDFLAGS) $< -o $@ $(ENV_ROOT)/lib/libhthread_device.a $(DSP_ROOT)/lib/vlib3000.a $(DSP_ROOT)/lib/slib3000.a

latency_dsp_device.o: latency_dsp_device.c
	$(DSP_ROOT)/bin/MT-3000-gcc $(CFLAGS) $< -o $@


Latency_MT:latency_check.cpp
	g++ latency_check.cpp  -std=c++17 -O3  -mtune=native -march=native -fopenmp -o Latency_MT

.PHONY: clean
clean:
	rm Latency_MT latency_dsp latency_dsp_device.o latency_dsp_device.out latency_dsp_device.dat
