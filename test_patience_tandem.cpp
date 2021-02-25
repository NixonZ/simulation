#include "includes.h"
#include "components/station.h"
#include "components/tandem.h"

float exponentialrv(float lambda)
{
    float U = random;
    return -log(U)/lambda;
}


void simulate_stations(tandem temp,event_type_list arrival_processes)
{    
    float least_dep_time = 0;
    int least_station_index = 0;

	std::tie(std::ignore,least_dep_time) = temp.find_least_dep_time();

    int discrete_events = 0;
    float t = 0;
    int arriving_customer = 0;
    int priority;
    std::vector<float> ta = call_event_type_list(arrival_processes);

    // temp.logger(t);
    temp.initialize_CSV("./output/tandem");

    while(discrete_events<100000)
    {
        std::tie(least_station_index, least_dep_time) = temp.find_least_dep_time();
        // temp.server_updates();
        t = std::min(least_dep_time, *std::min_element(ta.begin(),ta.end()) );

        temp.server_updates(t);

        if(t == *std::min_element(ta.begin(),ta.end()) )
        {
            priority = std::distance(ta.begin(), std::min_element(ta.begin(),ta.end()) );
            temp.add_customer_to_system(t,{priority,arriving_customer},true,arrival_processes,ta);
            arriving_customer++;
            ta[priority] = t + arrival_processes[priority](t) ;
        }
        else
            temp.departure_updates(least_station_index,t);

        if(discrete_events%10000==0 && discrete_events >0)
        {
            std::cout<<discrete_events <<'\n';
            std::cout<<"Writing to CSV\n";
            temp.dump_counter_variable_memory("./output/tandem");
        }

        // temp.print_tandemstatus(T(t));
        // temp.logger(t);
        // temp.logger(t);
        discrete_events++;
        // std::cout<<discrete_events<<endl;
    }
    // std::cout<<"Writing to CSV";
    // temp.write_to_csv("./output/tandem");
}

signed main()
{   
    std::cout << std::fixed << std::setprecision(precision);
    srand((unsigned)time(NULL));
    
   
    std::vector<float> lognormal_values_5_2 = read_csv("lognormal.csv",4);
    std::vector<float> lognormal_values_12_2 = read_csv("lognormal.csv",3);

    std::vector<station> station_list;

    station_list.push_back(
        station(1,1,{
            // 0
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
            },
            // 1
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
            }
        },2,event_type_list(2, [](double t)->double{ return 0; })
      )
    );
    
    //1
    station_list.push_back(
        station(2,2,{
            // 0
            [](float t)-> float { return exponentialrv(0.14); },
            // 1
            [](float t)-> float { return exponentialrv(0.13); }
        },2,event_type_list(2, [](double t)->double{ return 0; }))
    );

    //2 
    station_list.push_back(
        station(1,1,{
            //0
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
            },
            //1
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
            }
        },2,event_type_list(2, [](double t)->double{ return 0; }))
    );

    //3
    station_list.push_back(station(5,5,10,2,event_type_list(2, [](double t)->double{ return 0; })));

    //4
    station_list.push_back(station(4,4,5,2,event_type_list(2, [](double t)->double{ return 0; })));

    tandem temp(station_list,{
        //0
        [](float t) -> float{ return 5;},
        //1
        [](float t) -> float{ return 5;}
    });

    simulate_stations(temp,{
        //0
        [](float t)-> float { exponentialrv(0.0012); },
        //1
        [](float t)-> float { exponentialrv(0.011); }
    });
}