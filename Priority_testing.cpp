#include "includes.h"
#include "components/station.h"
#include "components/tandem.h"
#include "components/queue_graph.h"

float lambda(float t)
{
    if (t < 300.00)
        return 0.3 / 10.0;
    else if (t < 300 + 5 * 60)
        return (0.3 + (1.0 / (5.0 * 60.0)) * (t - 300.00)) / 10.00;
    else
        return (1.3 - (1.0 / (14.0 * 60.0)) * (t - 600)) / 10.00;
}

float Ts_generator(float s)
{
    float t = s;
    while (1)
    {
        float U = random;
        t -= log(U) / 1.3;
        U = random;
        if (U <= lambda( T(t) ) / 1.3)
            return t;
    }
}

float Normals(float mu, float sigma)
{
    // float U1 = random ;
    // float U2 = random;
    // float V1 = 2*U1 - 1;
    // float V2 = 2*U2 - 1;
    // float S = V1*V1 + V2*V2;
    // if(S>1)
    //   return Normals(mu,sigma);
    // pair<float,float> Normals;
    // Normals.first = (sqrt(-2.0*log(S)/S)*V1)*sigma + mu;
    // Normals.second = (sqrt(-2.0 * log(S) / S) * V1) * sigma + mu;
    // return min(Normals.first,Normals.second);
    float U = random;
    while (1)
    {
        if (U > 0.9)
        {
            U = random;
        }
        else
            break;
    }
    return (-log((1.0 / U) - 1) / 1.702) * sigma + mu;
}

float DepartureTimes(float t)
{
    if (t < 300.00)
        return Normals(30, 10);
    else if (t < 300 + 5 * 60)
        return Normals(10, 2);
    else
        return Normals(30, 10);
}

float exponentialrv(float lambda)
{
    float U = random;
    return -log(U)/lambda;
}

float priority_gen(int N)
{
    float U = random;
    return int(N*U);
}

void simulate_station(station temp)
{
    // 0 ,1 , 2, ... , N-1
    // temp.print_station_status(0);
    int discrete_events = 0;
    int arriving_customer = 0;
    float t = 0;
    float ta = exponentialrv(0.12);
    int priority = priority_gen(temp.num_classes());
    while (discrete_events < 500)
    {
        t = std::min(temp.find_min_td(), ta);
        temp.server_updates(t);
        if (t == ta)
        {
            //arrival happening
            priority = priority_gen(temp.num_classes());
            temp.add_customer_to_station(t, {priority,arriving_customer});
            arriving_customer++;
            ta = t + exponentialrv(0.12);
        }
        else
            temp.departure_updates(t);
        // temp.print_station_status(t);
        discrete_events++;
        temp.logger("lognormal",t);
    }
    temp.write_to_csv("./output/dataMG1");
}

void simulate_stations(std::vector<station> station_list)
{
    tandem temp (station_list);

    float least_dep_time = 0;
    int least_station_index = 0;

	std::tie(std::ignore,least_dep_time) = temp.find_least_dep_time();

    int discrete_events = 0;
    float t = 0;
    int arriving_customer = 0;
    int priority = priority_gen(temp.num_classes());
    float ta = Ts_generator(t);

    // temp.print_system_status(T(t));
    temp.logger(t);

    while(discrete_events<500)
    {
        std::tie(least_station_index, least_dep_time) = temp.find_least_dep_time();

        // std::cout << "Least dep time :" <<least_dep_time <<",Station_index : "<<least_station_index<<endl;

        t = std::min(least_dep_time, ta);

        temp.server_updates(t);

        if(t == ta)
        {
            // std::cout << "---------------------------------> Arrival at Station 0" << endl;
            // update first station for arrival (index =0 station)
            priority = priority_gen(temp.num_classes());
            temp.add_customer_to_system(t,{priority,arriving_customer});
            arriving_customer++;
            ta = Ts_generator(t);
            // std::cout << "Next arrival time :" << ta << endl;
        }
        else
            temp.departure_updates(least_station_index,t);

        // temp.print_tandemstatus(T(t));
        temp.logger(t);
        discrete_events++;
        std::cout<<discrete_events<<endl;
    }
    std::cout<<"Writing to CSV";
    temp.write_to_csv("./output/tandem");
}


void simulate_graph(graph station_graph)
{
    float least_dep_time = 0;
    int least_station_index = 0;

	std::tie(std::ignore,least_dep_time) = station_graph.find_least_dep_time();

    int discrete_events = 0;
    float t = 0;
    int arriving_customer = 0;
    float ta = Ts_generator(t);
    int priority = priority_gen(station_graph.num_classes());
    // temp.print_system_status(T(t));
    // temp.logger(t);

    while(discrete_events<500)
    {
        std::tie(least_station_index, least_dep_time) = station_graph.find_least_dep_time();

        t = std::min(least_dep_time, ta);

        station_graph.server_updates(t);

        if(t == ta)
        {
            priority = priority_gen(station_graph.num_classes());
            station_graph.add_customer_to_graph(t,{priority,arriving_customer});
            arriving_customer++;
            ta = Ts_generator(t);
        }
        else
            station_graph.departure_updates(least_station_index,t);
        
        station_graph.logger(t);
        discrete_events++;
        std::cout<<discrete_events<<endl;
    }
    std::cout<<"Writing to CSV";
    station_graph.write_to_csv("./output/graph");
}

int main()
{   
    srand((unsigned)time(NULL));
    // simulate_station(station(5,5,DepartureTimes,20, std::bind(priority_gen,20) ) );
    // auto c0 = std::bind(priority_gen,2);
    // std::cout<<c0() <<endl;

    std::vector<float> lognormal_values_5_2 = read_csv("lognormal.csv",4);
    std::vector<float> lognormal_values_12_2 = read_csv("lognormal.csv",3);

    std::vector<station> station_list;

    // 0
    station_list.push_back(station(1,1, 
    [lognormal_values_12_2](float t)-> float 
    { 
        float U = random; 
        int index = (int)(U*lognormal_values_12_2.size());
        try
        {
            return lognormal_values_12_2[index];
        }
        catch(const std::exception& e)
        {
            return lognormal_values_12_2[index-1];
        }
    },4
    ));

    //1
    station_list.push_back(station(1,1,
    [lognormal_values_12_2](float t)-> float 
    { 
        float U = random; 
        int index = (int)(U*lognormal_values_12_2.size());
        try
        {
            return lognormal_values_12_2[index];
        }
        catch(const std::exception& e)
        {
            return lognormal_values_12_2[index-1];
        }
    },4
    ));

    //2 
    station_list.push_back(station(1,1,
    [lognormal_values_12_2](float t)-> float 
    { 
        float U = random; 
        int index = (int)(U*lognormal_values_12_2.size());
        try
        {
            return lognormal_values_12_2[index];
        }
        catch(const std::exception& e)
        {
            return lognormal_values_12_2[index-1];
        }
    },4));

    //3
    station_list.push_back(station(5,5,DepartureTimes,4));

    //4
    station_list.push_back(station(4,4,
        [lognormal_values_5_2](float t)-> float 
        { 
            float U = random; 
            int index = (int)(U*lognormal_values_5_2.size());
            try
            {
                return lognormal_values_5_2[index];
            }
            catch(const std::exception& e)
            {
                return lognormal_values_5_2[index-1];
            }
        },4));

    std::vector< std::vector< std::pair<int,int> > > network = {
        {{1,1}},
        {{2,1},{3,1}},
        {{4,1}},
        {{4,1}},
        {}
    };
    // station_list.push_back(temp2);
    graph station_graph(0,0,network,station_list);
    simulate_graph(station_graph);

    return 0 ;
}