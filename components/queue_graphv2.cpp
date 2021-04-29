#include "queue_graphv2.h"

graphv2::graphv2(int init_N,int init_max_queue_len, std::vector<std::vector<std::pair<int, float>>> init_network, std::vector<station> temp, event_type_list PatienceTimes)
{
    N = init_N;
    max_queue_len = init_max_queue_len;
    bool check_network = true;
    float sum = 0;
    for (size_t i = 0; i < init_network.size(); i++)
    {
        sum = 0;
        if(init_network[i].empty())
        {
            exit_stations.push_back(i);
            continue;
        }
        for(auto& x:init_network[i])
        {
            sum += x.second;
        }
        if(sum<0.999 && init_network[i].empty() == 0)
        {
            std::cout<<"i:" <<i <<"sum" <<sum <<endl;
            check_network = false;
        }
    }
    assert(check_network == true);
    network = init_network;
    station_list = temp;
    num_nodes = station_list.size();
    this->PatienceTimes = PatienceTimes;
    // calculate_distance();
}

// void graph::calculate_distance()
// {
//     std::vector<bool> temp(num_nodes,0);
//     distance_from_exit[num_nodes-1] = 0;
//     for(int i=0;i<num_nodes;i++)
//     {
//         int v=-1;
//         for(int j=0;j<num_nodes;j++)
//         {
//             if(!temp[j] && (v==-1 || distance_from_exit[j] < distance_from_exit[v]))
//                 v=j ;
//         }
//         if(distance_from_exit[v]==INF)
//             break ;
//         temp[v]=1 ;
//         for(auto x : network[v])
//         {
//             int t = x.first ;
//             int len = x.second ;

//             if(distance_from_exit[v]+len < distance_from_exit[t])
//             {
//                 distance_from_exit[t]=distance_from_exit[v]+len ;
//             }
//         }
//     }
// }

void graphv2::server_updates(float t)
{
    for (auto &x : station_list)
    {
        x.server_updates(t);
    }
}

void graphv2::add_customer_to_graph(float t,customer arriving_customer)
{
    N++;
    station_list[0].add_customer_to_station(t, arriving_customer);
    system_counter_variable.push_back(std::make_tuple(arriving_customer, t, N, 0, 0));
}

void graphv2::add_customer_to_graph_vir(float t, customer curr_customer, bool keep_virtual, event_type_list arrival_processes, std::vector<float> ta)
{
    float virtual_wait_time = -1;
    // Generate virtual Waiting time by running a copy of the simulation
    float patience_time = PatienceTimes[curr_customer[0]](t);
    bool add = true;
    if(keep_virtual)
    {
        graphv2 virtual_wait( *this );
        int t_ = t;
        int arriving_customer = curr_customer[1];

        virtual_wait.add_customer_to_graph_vir(t_,curr_customer,false);
        arriving_customer++;
        ta[curr_customer[0]] = t_ + arrival_processes[curr_customer[0]](t_) ;
        float least_dep_time = 0;
        int least_station_index = 0;
        

        customer dept_cust;

        int priority;
        
        while (true)
        {
            std::tie(least_station_index, least_dep_time) = virtual_wait.find_least_dep_time();
            t_ = std::min(least_dep_time, *std::min_element(ta.begin(),ta.end()) );
            virtual_wait.server_updates(t_);

            
            if(t_ ==  *std::min_element(ta.begin(),ta.end()) )
            {
                priority = std::distance(ta.begin(), std::min_element(ta.begin(),ta.end()) );
                virtual_wait.add_customer_to_graph_vir(t_,{priority,arriving_customer},false);
                arriving_customer++;
                ta[priority] = t_ + arrival_processes[priority](t_) ;
            }
            else
            {
                dept_cust = virtual_wait.departure_updates(least_station_index,t_);
                if(dept_cust == curr_customer)
                    break;

            }
            if( t_-t > patience_time )
            {
                //  more time has elapsed and customer has not departed yet
                //thus virtual_wait_time > patience time
                add = false;
                break;
            }
            
        }
        virtual_wait_time = t_ - t;
    }
    // Comparision of patience times and decide whether to add customer
    if(add)
        add = (patience_time > virtual_wait_time);

    if(add)
    {
        N++;
        station_list[0].add_customer_to_station(t, curr_customer);
        
        system_counter_variable.push_back(std::make_tuple(curr_customer, t, N, 0, 0));
        
    }
    else
    {
        system_counter_variable.push_back(std::make_tuple(curr_customer, t, N, -1, -1));
    }
}

std::tuple<int, float> graphv2::find_least_dep_time()
{
    int station_index = 0;
    float least_dep_time = station_list[0].find_min_td();
    for (int i = 0; i < station_list.size(); i++)
    {
        if (least_dep_time > station_list[i].find_min_td())
        {
            least_dep_time = station_list[i].find_min_td();
            station_index = i;
        }
    }
    int server_index = station_list[station_index].find_min_k();
    return std::make_tuple(station_index, least_dep_time);
}

customer graphv2::departure_updates(int station_index, float t)
{
    if (std::find(exit_stations.begin(), exit_stations.end(), station_index) != exit_stations.end()) // can be binary searched
    {
        // std::cout << "---------------------------------> Departure at last station" << endl;
        N--;

        customer departing_customer = station_list[station_index].station::departure_updates(t);

        float service_time = 0;

        for (int i = station_list[station_index].get_counter_variable().size()-1;i>=0;i--)
        {
            if (departing_customer == std::get<0>(station_list[station_index].get_counter_variable()[i]))
            {
                service_time = std::get<4>(station_list[station_index].get_counter_variable()[i]);
                break;
            }
        }

        for (int i = system_counter_variable.size()-1;i>=0;i--)
        {
            if (departing_customer == std::get<0>(system_counter_variable[i]))
            {
                std::get<3>(system_counter_variable[i]) = service_time;
                std::get<4>(system_counter_variable[i]) = t;
                break;
            }
        }
        // do only departure updates for last station
        return departing_customer;
    }
    else
    {
        customer departing_customer = station_list[station_index].station::departure_updates(t);
        // do departure updates for station_index
        // arrival updates for one the connected stations.
    
        // int num_of_connected_nodes = (int)network[station_index].size();
        float U = random;
        float sum = 0;
        // Sample from the all the connected nodes (Discrete pdf)
        for(auto& x: network[station_index])
        {
            sum += std::get<1>(x); //cdf 
            if(U<sum)
            {
                station_list[std::get<0>(x)].add_customer_to_station(t,departing_customer);
                break;
            }
        }
        if(U==1)
            station_list[std::get<0>((*--network[station_index].end()))].add_customer_to_station(t,departing_customer);
    }
    return {1,1};
}

void graphv2::write_to_csv(std::string file_name = "data_system")
{
    std::ofstream data;

    data.open( file_name + ".csv", std::ofstream::out|std::ofstream::trunc);
    data << "Customer ID,Priority Level,Time of arrival,Number of people at arrival,Which 10 min interval in day,Which day in week,";
    // for(int i=0;i<this->station_list.size();i++)
    // {
    //     data << "Number of people at station " <<i <<" at arrival,";
    //     data << "Number of people in queue at station " <<i <<" at arrival,";
    //     data << "Number of active servers in station " <<i <<',';
    // }
    data <<"Time of start of service,Departure time,Wait time,\n";

    for (auto &x : system_counter_variable)
    {
        if(std::get<4>(x) == 0)
            continue;
        data << std::get<0>(x)[1] <<","
             << std::get<0>(x)[0] <<","
             << std::get<1>(x) <<","
             << std::get<2>(x) << ","
             << ((int(std::get<1>(x))) % 1440) / 10 << ','
             << int(std::get<1>(x) / 1440) % 7 << ',';

        // for(auto &station_info:std::get<3>(x))
        // {
        //     data << std::get<0>(station_info) <<',';
        //     data << std::get<1>(station_info) <<',';
        //     data << std::get<2>(station_info) <<',';
        // }


        data << std::get<3>(x) << ","
             << std::get<4>(x) << ","
             << (std::get<3>(x) - std::get<1>(x)) << ","
             << "\n";
    }
    data.close();
}

void graphv2::initialize_CSV(std::string tandem_name = "data_system")
{
    std::ofstream data;

    data.open( tandem_name + ".csv", std::ofstream::out|std::ofstream::trunc);
    data << "Customer ID,Priority Level,Time of arrival,Number of people at arrival,Which 10 min interval in day,Which day in week,";
    // for(int i=0;i<this->station_list.size();i++)
    // {
    //     data << "Number of people at station " <<i <<" at arrival,";
    //     data << "Number of people in queue at station " <<i <<" at arrival,";
    //     data << "Number of active servers in station " <<i <<',';
    // }
    data <<"Time of start of service,Departure time,Wait time,\n";
    data.close();
}

void graphv2::dump_counter_variable_memory(std::string tandem_name = "data_system")
{
    std::ofstream data;

    data.open( tandem_name + ".csv", std::ofstream::app);
    
    for (auto &x : system_counter_variable)
    {
        if(std::get<4>(x) == 0)
            continue;
        data << std::get<0>(x)[1] <<","
             << std::get<0>(x)[0] <<","
             << std::get<1>(x) <<","
             << std::get<2>(x) << ","
             << ((int(std::get<1>(x))) % 1440) / 10 << ','
             << int(std::get<1>(x) / 1440) % 7 << ',';

        // for(auto &station_info:std::get<3>(x))
        // {
        //     data << std::get<0>(station_info) <<',';
        //     data << std::get<1>(station_info) <<',';
        //     data << std::get<2>(station_info) <<',';
        // }


        data << std::get<3>(x) << ","
             << std::get<4>(x) << ","
             << ( ( std::get<4>(x) == -1 )?-1:(std::get<4>(x) - std::get<1>(x)) ) << ","
             << "\n";
    }
    
    std::vector<graph_data> system_counter_variable_temp;
    system_counter_variable_temp.assign(0,{});
    for(int i = 0;i<this->system_counter_variable.size();i++)
    {
        if(std::get<4>(this->system_counter_variable[i])==0)
            system_counter_variable_temp.push_back(this->system_counter_variable[i]);
    }
    this->system_counter_variable.clear();
    this->system_counter_variable = system_counter_variable_temp;

    for(int i =0 ;i<this->station_list.size();i++)
    {
        station_list[i].dump_counter_variable_memory();
    }

    data.close();
}

void graphv2::logger(float t)
{
    int l = 0;
    for (auto &x : station_list)
        x.logger(l++, t);
}

int graphv2::num_classes()
{
    return station_list[0].num_classes();
}