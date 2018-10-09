///////////////////////////////////////////
//
//	Keyboard send cmd_vel (unit: m/s,rad/s)
//	Cp from internet
//	Rewrite: CaoRou
//	Date: 2018.10.8
//
/////////////////////////////////////////// 
#include <termios.h>  
#include <signal.h>  
#include <math.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <sys/poll.h>  
#include <boost/thread/thread.hpp>  
#include <ros/ros.h>  
#include <geometry_msgs/Twist.h>  

#define KEYCODE_W 0x77  
#define KEYCODE_S 0x73  
#define KEYCODE_Q 0x71
#define KEYCODE_E 0x65  
#define KEYCODE_A 0x61  
#define KEYCODE_D 0x64 
#define KEYCODE_S_CAP 0x53  
#define KEYCODE_W_CAP 0x57  
#define KEYCODE_Q_CAP 0x51  
#define KEYCODE_E_CAP 0x45 
#define KEYCODE_A_CAP 0x41  
#define KEYCODE_D_CAP 0x44 

class KeyboardNode  
{  
private:  
	double walk_vel_;  
	double run_vel_;  
	double yaw_rate_;  
	double yaw_rate_run_;  
	geometry_msgs::Twist cmdvel_;  
	ros::NodeHandle n_;  
	ros::Publisher pub_;  

public: 
	KeyboardNode()  
	{ 
		pub_ = n_.advertise<geometry_msgs::Twist>("cmd_vel", 1);
		ros::NodeHandle n_private("~"); 
		n_private.param("walk_vel", walk_vel_, 1.0);  
		n_private.param("yaw_rate", yaw_rate_, 1.0);    
    } 
    ~KeyboardNode() { }  
    void keyboardLoop(); 
    void stopRobot()  
    {  
    	cmdvel_.linear.x = 0.0;  
		cmdvel_.linear.y = 0.0; 
    	cmdvel_.angular.z = 0.0;  
    	pub_.publish(cmdvel_);  
    }  
};  

 
int kfd = 0;  
struct termios cooked, raw;  
bool done; 

int main(int argc, char** argv)  
{  
    ros::init(argc,argv,"tbk", ros::init_options::AnonymousName | ros::init_options::NoSigintHandler);  
    KeyboardNode tbk;  
    boost::thread t = boost::thread(boost::bind(&KeyboardNode::keyboardLoop, &tbk));  
    ros::spin();  
    t.interrupt();  
    t.join();  
    tbk.stopRobot(); 
    tcsetattr(kfd, TCSANOW, &cooked);  
    return(0);  
}  

void KeyboardNode::keyboardLoop()  
{  
        char c;  
        double max_tv = walk_vel_;  
        double max_rv = yaw_rate_;  
        bool dirty = false;  
        int speed_x = 0;
  		int speed_y = 0;  
        int turn = 0;  
        // get the console in raw mode  
        tcgetattr(kfd, &cooked);  
        memcpy(&raw, &cooked, sizeof(struct termios));  
        raw.c_lflag &=~ (ICANON | ECHO);  
        raw.c_cc[VEOL] = 1;  
        raw.c_cc[VEOF] = 2;  
        tcsetattr(kfd, TCSANOW, &raw); 
        puts("Reading from keyboard");  
        puts("Use   W   keys to move the robot");  
		puts("Use A S D keys to move the robot"); 
        struct pollfd ufd;  
        ufd.fd = kfd;  
        ufd.events = POLLIN;  

        for(;;)  

        {  
            boost::this_thread::interruption_point();  
           // get the next event from the keyboard  
	    int num; 
            if ((num = poll(&ufd, 1, 250)) < 0)  
            {  
                perror("poll():");  
                return;  
            }  
            else if(num > 0)  
            {  
                if(read(kfd, &c, 1) < 0)  
                {  
                    perror("read():");  
                    return;  
                }  
            }  
            else  
            {  
                if (dirty == true)  
                {  
                    stopRobot();  
                    dirty = false;  
                } 
                continue;  
            }  
            switch(c)  
            {  
                case KEYCODE_W:  
                case KEYCODE_W_CAP:
                    max_tv = walk_vel_;  
                    speed_x = 1;  
                    speed_y = 0; 
                    turn = 0;  
                    dirty = true;  
                    break;  

                case KEYCODE_S:  
                case KEYCODE_S_CAP:
                    max_tv = walk_vel_;  
                    speed_x = -1;
                    speed_y = 0;  
                    turn = 0;  
                    dirty = true;  
                    break;  
                   
                case KEYCODE_A:
                case KEYCODE_A_CAP:  
                    max_rv = yaw_rate_;     
                    speed_x = 0;
                    speed_y = 0; 
                    turn = 1;  
                    dirty = true;  
                    break;

                case KEYCODE_D:
                case KEYCODE_D_CAP:  
                    max_rv = yaw_rate_;     
                    speed_x = 0;
                    speed_y = 0; 
                    turn = -1;  
                    dirty = true;  
                    break;

                default:  
                    max_tv = walk_vel_;  
                    max_rv = yaw_rate_;  
                    speed_x = 0; 
                    speed_y = 0; 
                    turn = 0;  
                    dirty = false;  
            }  

            cmdvel_.linear.x = speed_x * max_tv;  
            cmdvel_.linear.y = speed_y * max_tv;  
            cmdvel_.angular.z = turn * max_rv;  
            pub_.publish(cmdvel_);  
        }  

}

