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

class keyboard_not_ros
{
public:
	keyboard_not_ros();
	~keyboard_not_ros();
	void mainloop();
private:
	ShareMem *shareMem;
	pthread_mutex_t mutex;  
	void writeVel(double x, double y, double z);
};

keyboard_not_ros::keyboard_not_ros()
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
   		cout<<"stage_test haven't started"<<endl;
	}
}

keyboard_not_ros::~keyboard_not_ros()
{
	shmdt(shareMem);  //将共享内存段与当前进程脱离
	pthread_mutex_destroy(&mutex);
}

void keyboard_not_ros::mainloop()
{
	char c = 'a';
	while(c!='q')
	{
		cout<<"please input a letter"<<endl;
		c = getchar();
		getchar();
		double x,y,z;
		switch(c)
		{
			
			case 'W':
			case 'w':
				x = 1;
				y = 0;
				z = 0;
				break;
			case 'S':
			case 's':
				x = -1;
				y = 0;
				z = 0;
				break;
			case 'A':
			case 'a':
				x = 0;
				y = 0;
				z = 1;
				break;
			case 'D':
			case 'd':
				x = 0;
				y = 0;
				z = -1;
				break;
			default:;
		}
		writeVel(x, y, z);
		usleep(100*1000);
		writeVel(0,0,0);
	}
}

void keyboard_not_ros::writeVel(double x, double y, double z)
{
	pthread_mutex_lock(&mutex);
	shareMem->x = x;  
  	shareMem->y = y; 
  	shareMem->z = z;
	pthread_mutex_unlock(&mutex);
}


int main(int argc, char const *argv[])
{
	keyboard_not_ros k;
	k.mainloop();
	return 0;
}