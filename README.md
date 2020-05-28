# Simulation for delay prediction.
        
This repo is based on the paper [Accurate ED Wait Time Prediction by Erjie Ang et Al](https://web.stanford.edu/~bayati/papers/edwait.pdf). We are using simulated data and the model prescribed in the paper to study how wait times can be predicted.         
Further, we are also going to change the linear model to incorporate other ML techniques and hopefully create a better predictor.    

`Simulation`    
We follow a object oriented approach for creating a general Queueing System. Currently you can initialise the queue with n customers, and also add a new queue in between simulation. The departure time distributions and number of servers vs time need to be provided.    
All test cases can be found [here](Queue_class_test.cpp).


***
Project by-    
**Nalin Shani** 2018ME10057    
**Achintya Eeshan** 2018ME10094      
_B.Tech IIT Delhi Mechanical Engineering_
