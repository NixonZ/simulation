#include "station.h"
#include "tandem.h"
#ifndef GRAPH_H
#define GRAPH_H
// Directed Graphs only station_list[0] -> entry station_list[N-1] -> exit
class graph
{
    int num_nodes;
    int N ; //Number of custmer in system
    int max_queue_len;
    std::vector< std::vector< std::pair<int,int> > > network ; // { adjacent node,weight of edge to that node }
    std::vector<station> station_list; // 0 : Entry to system , -1 : Exit to station
    std::vector<graph_data> system_counter_variable;
    /* 
    0 - Customer
    1 - Time of arrival
    2 - Number of people in system at arrival
    3 - Service times at last station
    4 - Departure times of system
    */
public:
    graph(int init_N, int init_max_queue_len, std::vector<std::vector<std::pair<int, int>>> init_network, std::vector<station> temp);
    // void calculate_distance();
    std::tuple<int, float> find_least_dep_time();
    void add_customer_to_graph(float t, customer customer_id);
    void departure_updates(int station_index, float t);
    void server_updates(float t);
    void write_to_csv(std::string file_name);
    void initialize_CSV(std::string file_name);
    void dump_counter_variable_memory(std::string file_name);
    void logger(float t);
    int num_classes();
};
#endif