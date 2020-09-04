# Single Server Queue Simulation
Follow these basic steps to simulate a basic queueing system.
## Initialisation
Intialise the station class

    station (long init_mxN, C_type C_para, event_type dept_para, float t=0, int init_n=0)

    station (long init_mxN, C_type C_para, float init_dept, float t=0, int init_n=0)

    station (long init_mxN, int init_C, float init_dept, float t=0, int init_n=0)

    station (long init_mxN, int init_C, event_type init_dept, float t=0, int init_n=0)

Here `init_mxN` is the maximum number of servers in the station.
Here `C_type` is Function type `float -> int` and `event_type` is Function type `float -> float`.

You can either put **constant number of servers** in the system or create a function for number of server (**Dynamic servers**) in the  for example,

    int C(float t)
    {
        if (t < 130)
            return 8;
        else if (t < 7 * 60 + 30)
            return 5;
        else if (t < 10 * 60 + 30)
            return 6;
        else if (t < 22 * 60 + 30)
            return 9;
        else
            return 8;
    }

Similarly you can either put a constant service time for the servers or create a function which returns a random variable with a specified distribution for example,

    #define random (float)rand() / RAND_MAX;
    float exponentialrv(float lambda)
    {
        float U = random;
        return -log(U)/lambda;
    }
    // and then initialize the station.
    station temp(9, C, [](float t)->float 
    { 
        return exponentialrv(0.1);
    } );

**Note:** You can also use the inbuilt distribution in MS-Excel to generate random variables and then use them in your system.

    std::vector<float> lognormal_values = read_csv("lognormal.csv",1); // 1 is the column number

    station temp(1,1,
    [lognormal_values](float t)-> float 
    { 
        float U = random; 
        int index = (int)(U*lognormal_values.size());
        try
        {
            return lognormal_values[index];
        }
        catch(const std::exception& e)
        {
            return lognormal_values[index-1];
        }
    });

## Making time variable, counter variables, system state, event list
For discrete-event simulation we need create a global time variable, `t`.

    float t = 0;

The system state and counter variable for customers is defined in the station class:

    std::vector< std::tuple<int,float,int,int,float,float> > counter_variable; /*!< 
        0 - Customer id
        1 - Time of arrival
        2 - Number of people in system at arrival
        3 - Number of people in queue at arrival
        4 - Service times
        5 - Departure times
        */

But we need to create a new counter variable for the arriving customer.

    int arriving_customer = 0;

The event list for departures is kept in the station class and gets automatically updated after each departure.
The event list for arrivals needs to be created manually updated.

    float ta = exponentialrv(0.1);

Now we can move to running the actual simulation.
## Running the discrete-event simulation
We need to set the actual number of discrete events we want in our simulation.        

`int discrete_events = 0;`        

*Every code in this section is for a single discrete-event.* 
- Access the event list and get the minimum departure time of all servers and choose which event   is going to happen.

        std::tie(least_station_index, least_dep_time) = temp.find_least_dep_time();
        t = std::min(least_dep_time, ta);

- Run the server updates

        temp.server_updates(t);

- Depending on the event ( arrival or departure ), either add customer to the station or do the departure updates.

        if(t == ta)
        {
            temp.add_customer_to_system(t,arriving_customer);
            arriving_customer++;
            ta = Ts_generator(t);
        }
        else
            temp.departure_updates(least_station_index,t);

- Increase the number of events.

        discrete_events++;


In the end the loop should look this:

    while(discrete_events<5000)
    {
    std::tie(least_station_index, least_dep_time) = temp.find_least_dep_time();

    t = std::min(least_dep_time, ta);

    temp.server_updates(t);

    if(t == ta)
    {
        temp.add_customer_to_system(t,arriving_customer);
        arriving_customer++;
        ta = Ts_generator(t);
    }
    else
        temp.departure_updates(least_station_index,t);
        
    temp.logger(t);
    discrete_events++;
    std::cout<<discrete_events<<endl;
    }

**Note:** `temp.logger(t)` Beautifully outputs station status in a file in folder logs.
## Writing the System State in a CSV file
After simulation is over, write the counter variable and system state in a CSV.

    temp.write_to_csv("filename");


You can look at the class definition for more details of each function.

