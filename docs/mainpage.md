       
We follow a object oriented approach for creating a general Queueing System and getting simulated wait time data.
Further, we are also going to change the linear model to incorporate other ML techniques and hopefully create a better predictor. 

**Check out our code repository [here](https://github.com/NixonZ/simulation).**(Currently in work).

***
# Tutorials

Please visit our [examples](https://github.com/NixonZ/simulation/tree/master/examples) for various application.

Following are few examples exaplained briefly.

1. [Single Server Queue Simulation](md_docs_Single_Server_Queue.html)

***

`Simulation Models`         

We follow a object oriented approach for creating a general Queueing System. Three main classes are declared in the components folder:

1. [station](classstation.html) : A single queueing system. You can define the number of servers (even dynamic i.e. changing with time).

    Also you can provide custom departure time distribution by providing the inverse distribution function ( pass a function { float -> float } to station class that generates the random service times). 

    You'll have to use one the following constructor: 

        station (long init_mxN, C_type C_para, event_type dept_para, float t=0, int init_n=0)

        station (long init_mxN, C_type C_para, float init_dept, float t=0, int init_n=0)

        station (long init_mxN, int init_C, float init_dept, float t=0, int init_n=0)

        station (long init_mxN, int init_C, event_type init_dept, float t=0, int init_n=0)
        

    Here C_type is Function type float -> int and event_type is Function type float -> float. 

    Example of simple queueing system and more information on how to simulate can be found in examples folder.

2. [tandem](classtandem.html) : You can pass in a vector of station objects to group multiple queueing system in series.

3. [graph](classgraph.html) : Currently in work but the idea is to create a directed acyclic graph of station objects.

If you have already generated distribution for arrival or service times you can use,
    
    std::vector<float> read_csv(std::string filename,int index); //index is the column to be read.
    
to read the data out of a csv file.
and then the data can be used in our model in the following way:

    std::vector<float> service_times = read_csv("lognormal.csv",1);
    std::vector<float> interarrivaltimes = read_csv("lognormal.csv",2);

    station MG1(1,1,
    [service_times](float t) -> float
    {
        float U = random;
        int index = (int)(U*service_times.size());
        try
        {
            return service_times[index];
        }
        catch(const std::exception& e)
        {
            // std::cerr << e.what() << '\n'; 
            return service_times[index-1];
        }
    });

***
Project by-    

### Nalin Shani Nalin.Shani.me118@mech.iitd.ac.in     

### Achintya Eeshan me1180094@iitd.ac.in     

_B.Tech IIT Delhi Mechanical Engineering_
