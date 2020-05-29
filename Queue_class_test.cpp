#include "./simulation.h"
#include <bits/stdc++.h> 
#define random (float)rand() / RAND_MAX;
#define T(t) (t - int(t)) + int(t) % 1440
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

int C(float t)
{
    if (t < 10)
        return 3;
    else if (t < 120)
        return 2;
    else if (t < 7 * 60 + 30)
        return 1;
    else if (t < 10 * 60)
        return 2;
    else if (t < 10 * 60 + 20)
        return 3;
    else if (t < 14 * 60 + 30)
        return 4;
    else if (t < 18 * 60 + 30)
        return 5;
    else if (t < 20 * 60)
        return 4;
    else
        return 3;
}

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

void simulate_stations(std::vector<station> station_list)
{
    system_ temp (station_list);

    float least_dep_time = 0;
    int least_station_index = 0;

	std::tie(std::ignore,least_dep_time) = temp.find_least_dep_time();

    int discrete_events = 0;
    float t = 0;
    int arriving_customer = 0;
    float ta = Ts_generator(T(t));

    temp.print_system_status(T(t));

    while(discrete_events<50)
    {
        std::tie(least_station_index, least_dep_time) = temp.find_least_dep_time();

        // std::cout << "Least dep time :" <<least_dep_time <<",Station_index : "<<least_station_index<<endl;

        t = std::min(least_dep_time, ta);

        temp.server_updates(T(t));

        if(t == ta)
        {
            // std::cout << "---------------------------------> Arrival at Station 0" << endl;
            // update first station for arrival (index =0 station)
            temp.add_customer_to_system(T(t),arriving_customer);
            arriving_customer++;
            ta = Ts_generator(T(t));
            // std::cout << "Next arrival time :" << ta << endl;
        }
        else
            temp.departure_updates(least_station_index,T(t));

        temp.print_system_status(T(t));
        discrete_events++;
    }
}

void simulate_station(station temp)
{
    temp.print_station_status(0);
    int discrete_events = 0;
    int arriving_customer = 0;
    float t = 0;
    float ta = Ts_generator(T(t));
    while (discrete_events < 50)
    {
        t = std::min(temp.find_min_td(), ta);
        temp.server_updates(T(t));
        if (t == ta)
        {
            //arrival happening
            temp.add_customer_to_station(T(t), arriving_customer);
            arriving_customer++;
            ta = Ts_generator(T(t));
        }
        else
            temp.departure_updates(T(t));
        temp.print_station_status(T(t));
        discrete_events++;
    }
}

int main()
{
    srand((unsigned)time(NULL));

    std:: vector<station>station_list ;
    station_list.push_back( station(2,2,10) );
    station_list.push_back( station( 5, C , [](float t) -> float { return Normals(30, 10); } ) );
    station_list.push_back( station( 4, 4 , DepartureTimes  ) );

    simulate_stations(station_list) ;


    // Simulation for Single server 
    // station temp(5, [](float t)->int { return 5; } , [](float t)->float { return Normals(30,10); } ); //or
    // station temp(5,C,DepartureTimes,0,0);
    return 0 ;
}