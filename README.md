# roboquiz

The main package of the project is called `roboquiz_main`.

Run voice detection
```
rosrun ros_vosk vosk_node.py
```
Launch robotont driver
```
roslaunch robotont_driver fake_driver.launch
```
Running the main node
```
rosrun roboquiz_main main_game_node

```
Running the service node that gives a random question and answer pair to the client (`main_game_node` in this case)

```
rosrun roboquiz_main question_answer_service.py
```
