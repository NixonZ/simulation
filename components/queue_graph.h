#include "station.h"
#include "tandem.h"
#ifndef GRAPH_H
#define GRAPH_H
// Directed Graphs only station_list[0] -> entry station_list[N-1] -> exit
class graph
{
    int num_nodes;
    int N ; //Number of custmer is syste
    int max_queue_len;
    std::vector< std::vector< std::pair<int,int> > > network ; // {adjacent node,weight of edge to that node }
    std::vector<station> station_list;
    std::vector<bool> visited;
    std::vector<int> distance_from_exit;
    std::vector<std::tuple<int, float, int, float, float>> system_counter_variable;
    /* 
    0 - Customer id
    1 - Time of arrival
    2 - Number of people in system at arrival
    3 - Service times at last station
    4 - Departure times of system
    */
public:
    graph(int init_N, int init_max_queue_len, std::vector<std::vector<std::pair<int, int>>> init_network, std::vector<station> temp);
    void calculate_distance();
    void min_queue_len(int v=0);
    std::tuple<int, float> find_least_dep_time();
    std::vector<int> dfs2(int v=0);
    void add_customer_to_graph(float t, int customer_id);
    void departure_updates(int station_index, float t);
};
#endif