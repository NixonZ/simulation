#include "queue_graph.h"

graph::graph(int init_N,int init_max_queue_len, std::vector<std::vector<std::pair<int, int>>> init_network, std::vector<station> temp)
{
    N = init_N;
    max_queue_len = init_max_queue_len;
    network = init_network;
    station_list = temp;
    num_nodes = station_list.size();
    distance_from_exit.assign(num_nodes,INF);
    calculate_distance();
}

void graph::calculate_distance()
{
    std::vector<bool> temp(num_nodes,0);
    distance_from_exit[num_nodes-1] = 0;
    for(int i=0;i<num_nodes;i++)
    {
        int v=-1;
        for(int j=0;j<num_nodes;j++)
        {
            if(!temp[j] && (v==-1 || distance_from_exit[j] < distance_from_exit[v]))
                v=j ;
        }
        if(distance_from_exit[v]==INF)
            break ;
        temp[v]=1 ;
        for(auto x : network[v])
        {
            int t = x.first ;
            int len = x.second ;

            if(distance_from_exit[v]+len < distance_from_exit[t])
            {
                distance_from_exit[t]=distance_from_exit[v]+len ;
            }
        }
    }
}

void graph::add_customer_to_graph(float t,int arriving_customer)
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

        int departing_customer = station_list[station_index].station::departure_updates(t);

        float service_time = 0;

        for (auto &x : station_list[station_index].get_counter_variable())
        {
            if (departing_customer == std::get<0>(x))
                service_time = std::get<4>(x);
        }

        for (auto &x : system_counter_variable)
        {
            if (departing_customer == std::get<0>(x))
            {
                std::get<3>(x) = service_time;
                std::get<4>(x) = t;
            }
        }
        // do only departure updates for last station
    }
    else
    {
        station_list[station_index].departure_updates(t)
        // do departure updates for station_index
        // do arrival updates for station_index+1
    }
}

void graph::min_queue_len(int v=0)
{
    visited[v]= true ;
    int min_len= std::min(min_len,station_list[v].find_queue_len());
    for(auto x: network[v])
        min_queue_len(x.first);
}
std::vector<int> graph::dfs2(int v=0)
{
    visited[v]=1 ;
}
