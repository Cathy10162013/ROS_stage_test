/****************************************
*文件名: talker.cpp			*
*简介: 链接非ROS进程和ROS节点的ROS节点	*
*BY: CR (qq: 414481619)			*
*创建时间: 2018.10.09			*
****************************************/

#include "ros/ros.h"
#include <geometry_msgs/Twist.h>

#include <iostream>
#include <stdio.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>

#include "sharemem.h"

using namespace std;


int main(int argc, char **argv)
{

  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

  ros::Rate loop_rate(100); // 100Hz的定时器

  
  ShareMem _ShareMem;
  // 创建共享内存 ，相当于打开文件，文件不存在则创建
  int shmid = shmget((key_t)2333, sizeof(_ShareMem), 0666|IPC_CREAT);  
  if(shmid==-1)
  {
    perror("shmget err");
    return errno;
  }
  cout<<shmid<<endl; 

  // 将共享内存段连接到进程地址空间, 第二个参数shmaddr为NULL，核心自动选择一个地址
  ShareMem *shareMem = (ShareMem *)shmat(shmid, NULL, 0); 
  if (shareMem == (void *)-1 )
  {
    perror("shmat err");
    return errno;
  }
  
  shareMem->x = 0;  
  shareMem->y = 0; 
  shareMem->z = 0; 

  pthread_mutex_t mutex;  
  pthread_mutex_init(&mutex, NULL);
  while (ros::ok())
  {
    geometry_msgs::Twist cmdvel_msg; 

    pthread_mutex_lock(&mutex);
    cmdvel_msg.linear.x = shareMem->x;  
    cmdvel_msg.linear.y = shareMem->y; 
    cmdvel_msg.angular.z = shareMem->z; 
    pthread_mutex_unlock(&mutex);
    chatter_pub.publish(cmdvel_msg);

    ros::spinOnce();

    loop_rate.sleep();
  }
  pthread_mutex_destroy(&mutex);
  shmdt(shareMem);  //将共享内存段与当前进程脱离
  shmctl(shmid, IPC_RMID, NULL); //IPC_RMID为删除内存段
  return 0;
}
