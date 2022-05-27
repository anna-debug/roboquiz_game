import rospy
import numpy as np
from roboquiz_main.srv import GiveQuestionAnswer, GiveQuestionAnswerResponse

old_questions = []
question_list = []
load_questions = True

def get_random_pair(req):
    
    global old_questions, question_list, load_questions

    if req.f == 1:
        f = open("/home/peko/catkin_ws/src/roboquiz_main/quiz", "r")
        lines = f.readlines()

    q_a_pairs = {}

    for line in lines:
        line = line.strip()
        question = line.split(":")[0]
        answer = line.split(":")[1]
        q_a_pairs[question] = answer

    if load_questions == True:
        questions = q_a_pairs.keys()
        question_list = list(questions)
        load_questions = False
    
    
    if len(question_list) != 0:
        i = np.random.randint(0,len(question_list))
        r_question = question_list[i]
        r_answer = q_a_pairs[question_list[i]]
        old_questions.append(question_list[i])
        question_list.pop(i)
        
    
    else:
        question_list = old_questions
        old_questions = []
        i = np.random.randint(0,len(question_list))
        r_question = question_list[i]
        r_answer = q_a_pairs[question_list[i]]

        print("question change")

    return GiveQuestionAnswerResponse(r_question, r_answer)
    
def get_random_pair_server():
    
    rospy.init_node("get_random_pair_server")
    s = rospy.Service("give_question_answer", GiveQuestionAnswer, get_random_pair)
    rospy.spin()

if __name__ == "__main__":
    get_random_pair_server()


    


