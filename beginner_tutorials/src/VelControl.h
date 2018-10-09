/****************************************
*文件名: VelControl.h			*
*简介: 速度控制父类			*
*BY: CR (qq: 414481619)			*
*创建时间: 2018.10.09			*
****************************************/

// this code ism't in ros system 

#include <iostream>
#include <stdio.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "sharemem.h"

using namespace std;

class VelControl
{
public:
	VelControl();
	~VelControl();
	void mainloop();
protected:
	ShareMem *shareMem;
	pthread_mutex_t mutex;  
	void writeVel(double x, double y, double z);
};

VelControl::VelControl()
{
  	pthread_mutex_init(&mutex, NULL);

  	// 绑定共享内存
	int shmid = shmget((key_t)2333, 0, 0);  
	if(shmid != -1)
	{
		shareMem =(ShareMem *)shmat(shmid, NULL, 0);
		cout<<shmid<<endl;
	}
	else
	{
   		cout<<"ERROR: stage_test haven't started"<<endl;
		exit(1);
	}
}

VelControl::~VelControl()
{
	shmdt(shareMem);  //将共享内存段与当前进程脱离
	pthread_mutex_destroy(&mutex);
}


void VelControl::writeVel(double x, double y, double z)
{
	pthread_mutex_lock(&mutex);
	shareMem->x = x;  
  	shareMem->y = y; 
  	shareMem->z = z;
	pthread_mutex_unlock(&mutex);
}


