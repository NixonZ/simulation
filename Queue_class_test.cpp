#include "./station.h"
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
void simulate_stations(std::vector<station> system){

	std::vector<float>min_departure_time(system.size()) ;
	int station_index = 0;
    float least_dep_time = system[0].find_min_td() ;
	for(int i=0;i<system.size();i++)
    {
		min_departure_time[i]= system[i].find_min_td() ;
		if(least_dep_time>min_departure_time[i])
        {
			least_dep_time = min_departure_time[i] ;
			station_index =i ;
		}
	}
	int server_index = system[station_index].find_min_k() ;
    int discrete_events = 0;
    float t = 0;
    float ta = Ts_generator(T(t));
    int l=0;
    for (auto &x : system)
    {
        std::cout << "#### Station No." << l++ << "####" << endl;
        x.print_station_status(T(t));
    }
    while(discrete_events<10)
    {
        l=0;
        least_dep_time = system[0].find_min_td();
        station_index = 0;
        for (int i = 0; i < system.size(); i++)
        {
            min_departure_time[i] = system[i].find_min_td();
            if (least_dep_time > min_departure_time[i])
            {
                least_dep_time = min_departure_time[i];
                station_index = i;
            }
        }
        std::cout << "Least dep time :" <<least_dep_time <<",Station_index : "<<station_index<<endl;
        t = std::min(least_dep_time, ta);
        for(auto &x: system)
        {
            x.server_updates(T(t));
        }
        int l=0;
        if(t == ta)
        {
            std::cout << "---------------------------------> Arrival at Station 0" << endl;
            // update first station for arrival (index =0 station)
            system[0].add_customer_to_station( T(t) );
            ta = Ts_generator(T(t));
            std::cout << "Next arrival time :" << ta << endl;
        }
        else
        {
            if(station_index == system.size()-1)
            {
                std::cout << "---------------------------------> Departure at last station" << endl;
                system[station_index].departure_updates(T(t)) ;
                // do only departure updates for last station
            }
            else
            {
                std::cout << "---------------------------------> Departure at station :" <<station_index << endl;
                system[station_index].departure_updates(T(t));
                system[station_index+1].add_customer_to_station(T(t)) ;
                // do departure updates for station_index 
                // do arrival updates for station_index+1 
            }
        }
        for (auto &x : system)
        {
            std::cout << "#### Station No." << l++ << "####" << endl;
            x.print_station_status(T(t));
        }
        discrete_events++;
    }
}

int main()
{
    srand((unsigned)time(NULL));
    std:: vector<station>station_list ;
    // for(int i=0;i<2;i++)
    station_list.push_back( station( 2, [](float t)->int { return 2; } , [](float t)->float { return Normals(30,10); } ) );
    station_list.push_back( station( 5, C , [](float t) -> float { return Normals(30, 10); } ) );
    station_list.push_back( station( 4, [](float t) -> int { return 4; } , DepartureTimes  ) );
    simulate_stations(station_list) ;
    // int l=0;
    // int t =0;
    // for (auto &x : station_list)
    //     {
    //         std::cout <<"#### Station No." <<l++  <<"####" <<endl ;
    //         x.print_station_status(T(t));
    //     }
    return 0 ;
    // Simulation for Single server 
    // station temp(5, [](float t)->int { return 5; } , [](float t)->float { return Normals(30,10); } );
    // station temp(5,C,DepartureTimes,0,0);
    // temp.print_station_status(0);
    // int discrete_events = 0;
    // float t = 0;
    // float ta = Ts_generator(T(t));
    // while (discrete_events<10)
    // {
    //     t = std::min(temp.find_min_td(), ta);
    //     temp.server_updates( T(t) );
    //     if (t == ta)
    //     {
    //         //arrival happening
    //         temp.add_customer_to_station( T(t) );
    //         ta = Ts_generator( T(t) );
    //     }
    //     else
    //         temp.departure_updates( T(t) );
    //     temp.print_station_status( T(t) );
    //     discrete_events++;
    // }
}