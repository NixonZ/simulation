#include "station.h"
#ifndef TANDEM_H
#define TANDEM_H
class tandem
{
    std::vector<station> station_list;
    int number_of_station;
    std::vector<std::tuple<int, float, int, float, float>> system_counter_variable;
    /* 
    0 - Customer id
    1 - Time of arrival
    2 - Number of people in system at arrival
    3 - Service times at last station
    4 - Departure times of system
    */
    int N; //Number of people in system

public:
    tandem(std::vector<station> temp);
    std::tuple<int, float> find_least_dep_time();
    void print_system_status(float t);
    void add_customer_to_system(float t, int arriving_customer);
    void departure_updates(int station_index, float t);
    void server_updates(float t);
    void write_to_csv(std::string tandem_name = "data_system");
    void logger(float t);
};
#endif