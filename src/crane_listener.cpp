// %Tag(FULLTEXT)%
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "evart_bridge/TrackSegment.h"
#include "crane_message_handler.hh"

// %EndTag(CALLBACK)%

void init_ros(ros::NodeHandle &n,
	      CraneMessageHandler &craneMsgHandler)
{

  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to the subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */
  // %Tag(SUBSCRIBER)%

  // Call evart to follow HRP2.
  ros::ServiceClient client = n.serviceClient<evart_bridge::TrackSegment>("/evart/track_segments");

  evart_bridge::TrackSegment trackASegment; 
  
  trackASegment.request.body_name="hrp2_head_sf";
  trackASegment.request.segment_name="PO";

  unsigned int res_to_call;

  if (res_to_call=client.call(trackASegment))
    {
      std::cout << "Succeeded in subscribing to track_segment:" 
		<< trackASegment.request.body_name << "/" 
		<< trackASegment.request.segment_name 
		<< std::endl;
    }
  else
    {
      std::cerr << "Failed to subscribe to " 
		<< trackASegment.request.body_name << "/" 
		<< trackASegment.request.segment_name << std::endl;
    }

  craneMsgHandler.setMocapStatus(res_to_call);
  
}


int main(int argc, char **argv)
{

  CraneMessageHandler craneMsgHandler;
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line. For programmatic
   * remappings you can use a different version of init() which takes remappings
   * directly, but for most command-line programs, passing argc and argv is the easiest
   * way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "listener");
  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;


  init_ros(n,craneMsgHandler);

  ros::Subscriber sub = n.subscribe("joy", 1, 
				    &CraneMessageHandler::chatterCallback,
				    &craneMsgHandler); 

  //sub = n.subscribe("/evart/facom_box/PO",1000,&CraneMessageHandler::updatePositionCallback,&craneMsgHandler);
  sub = n.subscribe("/evart/hrp2_head_sf/PO",1000,&CraneMessageHandler::updatePositionCallback,&craneMsgHandler);
  // %EndTag(SUBSCRIBER)%

  ros::Rate loop_rate(50);

  int count = 0;

  while (ros::ok())
  {
      
      /**
       * ros::spin() will enter a loop, pumping callbacks.  With this version, all
       * callbacks will be called from within this thread (the main one).  ros::spin()
       * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
       */
      // %Tag(SPIN)%
      // %EndTag(SPIN)%
  
      ros::spinOnce();
      // %EndTag(SPINONCE)%

      // Control the crane
      craneMsgHandler.applyControlStrategy();

      // %Tag(RATE_SLEEP)%
      loop_rate.sleep();
      // %EndTag(RATE_SLEEP)%
      ++count;
    }

  return 0;
}
// %EndTag(FULLTEXT)%
