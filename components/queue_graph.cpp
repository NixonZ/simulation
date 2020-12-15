#include "queue_graph.h"

graph::graph(int init_N,int init_max_queue_len, std::vector<std::vector<std::pair<int, int>>> init_network, std::vector<station> temp)
{
    N = init_N;
    max_queue_len = init_max_queue_len;
    network = init_network;
    station_list = temp;
    num_nodes = station_list.size();
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

void graph::server_updates(float t)
{
    for (auto &x : station_list)
    {
        x.server_updates(t);
    }
}

void graph::add_customer_to_graph(float t,customer arriving_customer)
{
    N++;
    station_list[0].add_customer_to_station(t, arriving_customer);
    system_counter_variable.push_back(std::make_tuple(arriving_customer, t, N, 0, 0));
}

std::tuple<int, float> graph::find_least_dep_time()
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

void graph::departure_updates(int station_index, float t)
{
    if (station_index == station_list.size() - 1)
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
    }
    else
    {
        customer departing_customer = station_list[station_index].station::departure_updates(t);
        // do departure updates for station_index
        // arrival updates for one the connected stations.
    
        int num_of_connected_nodes = (int)network[station_index].size();
        float U = random;
        station_list[std::get<0>(network[station_index][int(num_of_connected_nodes*U)])].add_customer_to_station(t,departing_customer);
    }
}

void graph::write_to_csv(std::string file_name = "data_system")
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

void graph::initialize_CSV(std::string tandem_name = "data_system")
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

void graph::dump_counter_variable_memory(std::string tandem_name = "data_system")
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
             << (std::get<3>(x) - std::get<1>(x)) << ","
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

void graph::logger(float t)
{
    int l = 0;
    for (auto &x : station_list)
        x.logger(l++, t);
}

int graph::num_classes()
{
    return station_list[0].num_classes();
}