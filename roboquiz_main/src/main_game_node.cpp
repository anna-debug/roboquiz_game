#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h" 
#include <algorithm>
#include <fstream>
#include "roboquiz_main/GiveQuestionAnswer.h"
#include <sound_play/sound_play.h>
#include <unistd.h>
#include <map>
#include <ctime>

std_msgs::String voice;
std::string question;
std::string answer;
std::vector<std::string> player_names;
bool process_answer = false;
bool ask_question = false;
bool initial_state = true;
bool ask_names = true;
std::map<std::string, int> points;
std::time_t stop_time;
std::time_t start_time;
std::time_t start_time2;

void getVoiceInput(std_msgs::String msg)
{ 
    voice = msg;
    process_answer = true;
    ROS_INFO("Command received");
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "main_game_node");

    ros::NodeHandle n;

    sound_play::SoundClient sc;
    ros::Duration(2, 0).sleep();

    ros::ServiceClient client = n.serviceClient<roboquiz_main::GiveQuestionAnswer>("give_question_answer");
    ros::Subscriber sub = n.subscribe("speech_recognition/final_result", 100, getVoiceInput);
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 100);

    roboquiz_main::GiveQuestionAnswer srv;
    srv.request.f = 1;   
    client.waitForExistence();
    

    if (client.call(srv)){
        question = srv.response.q;
        answer = srv.response.a;
        ROS_INFO("Reiceived the question and answer pair");
        ask_question = true;
    }

    ros::Rate loop_rate(10);
    int count = 0;
    ros::spinOnce();
    int state = 0;
    int name_num = 0;
    start_time2 = time(NULL);

    while (ros::ok()){
        //std::cout << "number of names " << name_num;
        //std::cout << "player names size " << player_names.size();
        
        if (name_num >= player_names.size()){
            ROS_INFO ("Changing name_num number");
            name_num = 0;
        }
        
        if (initial_state == true)
        {
            ROS_INFO("Initial state");
            if (ask_names == true){
                ros::Duration(7, 0).sleep();
                sc.say("Welcome to roboquiz. Players, register your names one by one. If something goes wrong say reset. To stop the registration and start the game say stop");
                ask_names = false;
            }

            if (process_answer == true){
                int index_str;
                std::string str_voice = boost::lexical_cast<std::string>(voice);
                str_voice.erase(0,6);
                
                

                if (((index_str = str_voice.find("top")) != -1) || ((index_str = str_voice.find("stop")) != -1) || ((index_str = str_voice.find("gop")) != -1)){
                    stop_time = time(NULL);
                    ROS_INFO("Stoping name gathering");
                    sc.say("Player registration has ended. Let the game begin");
                    ros::Duration(6, 0).sleep();
                    initial_state = false;
                    process_answer = false;
                    

                }
                else if((index_str = str_voice.find("reset"))!= -1){
                    ROS_INFO("Reset");
                    sc.say("Reseting player names");
                    player_names.clear();
                    points.clear();
                    process_answer = false;
                }
                    
                else{
                    if (player_names.size() >= 6){
                        sc.say("Sorry, no more than six players");
                    }
                    else{
                        player_names.insert(player_names.begin(), str_voice);
                        points.insert(std::pair<std::string,int>(str_voice,0));
                        process_answer = false;
                        sc.say("Player "+str_voice+" has been registered");
                    }
                    
                }
            }
        } 

        else if (initial_state == false)
        {
            
            ROS_INFO("Publisher set");
            geometry_msgs::Twist msg;
            msg.angular.z = 7;
            pub.publish(msg);
            stop_time = time(NULL);

            if ((stop_time - start_time) > 5){

                std::string str_voice = boost::lexical_cast<std::string>(voice);
                str_voice.erase(0,6);
                int index_str;

                //std::cout << question;
                //std::cout << str_voice;
                if (ask_question == true){
                    sc.say(player_names[name_num]+" "+question);
                    ROS_INFO("i just asked a question");
                }

                if (process_answer == true){

                    if ((index_str = str_voice.find("game over")) != -1){
                        for (const auto & pair:points) {
                            std::string value = boost::lexical_cast<std::string>(pair.second);
                            sc.say(pair.first+" has "+value+" points");
                            ros::Duration(3.0).sleep();
                        }
                        break;
                    }
                    
                    else if ((index_str = str_voice.find(answer)) != -1){
                        ROS_INFO("Your answer was correct!");
                        sc.playWave("/home/peko/catkin_ws/src/roboquiz_main/sounds/correct_answer.wav");
                        points[player_names[name_num]] ++;
                        msg.angular.z = 0;
                        pub.publish(msg);
                        process_answer = false;
                        ask_question = true;
                        ++name_num;

                        if (client.call(srv)){
                            question = srv.response.q;
                            answer = srv.response.a;
                        }
                        ros::Duration(3.0).sleep();
                    }

                    else if ((index_str = str_voice.find(answer)) == -1){
                        ROS_INFO("Your answer was incorrect!");
                        sc.playWave("/home/peko/catkin_ws/src/roboquiz_main/sounds/wrong_answer.wav");
                        sc.say(" ");
                        sc.say(" ");
                        sc.say(" The correct answer is "+answer);
                        process_answer = false;
                        ask_question = true;
                        ++name_num;

                        if (client.call(srv)){
                            question = srv.response.q;
                            answer = srv.response.a;
                        }
                        start_time = time(NULL);
                        //ros::Duration(3, 0).sleep();
                    }
                }

                else{
                    ask_question = false;
                }
            }
        }

        
        ros::spinOnce();
        loop_rate.sleep();
        ++count;
    } 
    return 0;
}





