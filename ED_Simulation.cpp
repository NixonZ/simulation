#include "components/tandem.h"

int C_doctors(float t)
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

int C_nurses(float t)
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
    tandem temp (station_list);

    float least_dep_time = 0;
    int least_station_index = 0;

	std::tie(std::ignore,least_dep_time) = temp.find_least_dep_time();

    int discrete_events = 0;
    float t = 0;
    int arriving_customer = 0;
    float ta = Ts_generator(t);

    // temp.print_system_status(T(t));
    // temp.logger(t);

    while(discrete_events<1e5)
    {
        std::tie(least_station_index, least_dep_time) = temp.find_least_dep_time();

        // std::cout << "Least dep time :" <<least_dep_time <<",Station_index : "<<least_station_index<<endl;

        t = std::min(least_dep_time, ta);

        temp.server_updates(t);

        if(t == ta)
        {
            // std::cout << "---------------------------------> Arrival at Station 0" << endl;
            // update first station for arrival (index =0 station)
            temp.add_customer_to_system(t,arriving_customer);
            arriving_customer++;
            ta = Ts_generator(t);
            // std::cout << "Next arrival time :" << ta << endl;
        }
        else
            temp.departure_updates(least_station_index,t);

        // temp.logger(t);
        discrete_events++;
        std::cout<<discrete_events<<endl;
    }
    std::cout<<"Writing to CSV\n";
    temp.write_to_csv("ED-Simulation");
}

int main()
{
    srand((unsigned)time(NULL));

    std::vector<float> lognormal_values_5_3 = read_csv("lognormal.csv",2);
    std::vector<float> lognormal_values_10_2 = read_csv("lognormal.csv",3);
    
    std::vector<station> station_list ;

    station_list.push_back( station( 9, C_nurses,
     [lognormal_values_5_3](float t) -> float
     { 
        float U = random; 
        int index = (int)(U*lognormal_values_5_3.size());
        try
        {
            return lognormal_values_5_3[index];
        }
        catch(const std::exception& e)
        {
            return lognormal_values_5_3[index-1];
        } 
     }
    ));

    station_list.push_back( station( 5, C_doctors, 
     [lognormal_values_10_2](float t) -> float
     {
        float U = random; 
        int index = (int)(U*lognormal_values_10_2.size());
        try
        {
            return lognormal_values_10_2[index];
        }
        catch(const std::exception& e)
        {
            return lognormal_values_10_2[index-1];
        }
     }
    ));
    simulate_stations(station_list);
}