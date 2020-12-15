#include "includes.h"
#include "components/station.h"

float exponentialrv(float lambda)
{
    float U = random;
    return -log(U)/lambda;
}


void simulate_station(station temp,event_type_list arrival_processes)
{
    int discrete_events = 0;
    int arriving_customer = 0;
    float t = 0;
    // std::vector<float> ta = std::for_each(arrival_processes.begin(),arrival_processes.end(), event_type::operator() );
    std::vector<float> ta = call_event_type_list(arrival_processes);
    int priority;
    // temp.initialize_CSV("./output/dataMM1_patience");
    while (arriving_customer < 10)
    {
        t = std::min( temp.find_min_td(),  *std::min_element(ta.begin(),ta.end()) );
        temp.server_updates(t);
        if (t == *std::min_element(ta.begin(),ta.end()))
        {
            //arrival happening
            priority = std::distance(ta.begin(), std::min_element(ta.begin(),ta.end()) );
            temp.add_customer_to_station(t, {priority,arriving_customer},true,arrival_processes,ta);
            arriving_customer++;
            ta[priority] = t + arrival_processes[priority](t) ;
        }
        else
            temp.departure_updates(t);
        // temp.print_station_status(t);

        // if(arriving_customer%10000==0)
        // {
        //     // std::cout<<"Writing to CSV\n";
        //     std::cout<<discrete_events <<endl;
        //     temp.dump_counter_variable_memory("./output/dataMM1_patience");
        // }

        discrete_events++;
        temp.logger("MM1",t);
    }
    // temp.write_to_csv("./output/dataMM1");
}

signed main()
{   
    std::cout << std::fixed << std::setprecision(precision);
    srand((unsigned)time(NULL));
    
    simulate_station(
        station(1,1,
            event_type_list({
                // 0
                [](float t)-> float
                {
                    return exponentialrv(0.14);
                },
                // 1
                [](float t)-> float
                {
                    return exponentialrv(0.13);
                },
                // 2
                [](float t)-> float
                {
                    return exponentialrv(0.20);
                }
            })
        ,3,event_type_list(3, [](double t)->double{ return 2;}))
    ,
    //Arrival processes
        {
            // 0
            [](float t)-> float { exponentialrv(1.2/1000); },
            // 1
            [](float t)-> float { exponentialrv(1.1/100); },
            // 2
            [](float t)-> float { exponentialrv(0.9/10); },
        }
    );

}