/****************************************
*文件名: keyboard_not_ros.cpp		*
*简介: 速度控制例程			*
*BY: CR (qq: 414481619)			*
*创建时间: 2018.10.09			*
****************************************/

// this code isn't in ros system 

#include "VelControl.h"

using namespace std;

class keyboard_not_ros:VelControl
{
public:
	void mainloop();
};

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


int main(int argc, char const *argv[])
{
	keyboard_not_ros k;
	k.mainloop();
	return 0;
}
