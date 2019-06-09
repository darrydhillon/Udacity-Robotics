#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <vector>

using std::cout;
using std::endl;

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Moving towards the ball");
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv)) {
      ROS_ERROR("Failed to call service drive_robot");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
	cout << img.step << endl;
	int white_pixel = 255;
	double width = img.width; 
	std::vector<int> myVector;
	for(double i = 0; i < img.height; i++)
		{
		for(double j = 0; j < img.step; j+=3)
			{
			if ((int)img.data[i*img.step+j] == white_pixel)
			myVector.push_back(j/3);
			}
		}
	if (myVector.size() == 0)
		{
		drive_robot(0.0, 0.0);
		return;
		}
	double myVector_avg = std::accumulate(myVector.begin(), myVector.end(), 0)/myVector.size(); 
	if (myVector_avg < width / 3)
		{
		drive_robot(0.1, 0.1);
		}
		else if (myVector_avg < 2 * width /3)
			{
			drive_robot(0.1, 0.0);
			}
			else 
			{
			drive_robot(0.1, -0.1);
			}
	return;
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
