#include "station.h"
#ifndef TANDEM_H
#define TANDEM_H
class tandem
{
    std::vector<station> station_list;
    int number_of_station;
    std::vector<tandem_data> system_counter_variable;
    event_type_list PatienceTimes;
    int num_class;
    /* 
    0 - Customer
    1 - Time of arrival
    2 - Number of people in system at arrival
    3 - Vector containing information about all the stations
    3 - Service times at last station
    4 - Departure times of system
    */
    int N; //Number of people in system

public:
    tandem(std::vector<station> temp, event_type_list PatienceTimes_para = event_type_list( 1 , [](float t) -> float { return (0.0); }  ),int num_classes_para = 0)
    : PatienceTimes(PatienceTimes_para),num_class(num_classes_para)
    {
        station_list = temp;
        number_of_station = temp.size();
        N = 0;
    }
    tandem(const tandem&copy)
    {
        this->station_list = copy.station_list;
        this->number_of_station = copy.number_of_station;
        this->PatienceTimes = copy.PatienceTimes;
        this-> num_class = copy.num_class;
    }
    std::tuple<int, float> find_least_dep_time();
    void print_system_status(float t);
    void add_customer_to_system(float t,customer curr_customer,bool keep_virtual = false, event_type_list arrival_processes = { [](float t)-> float{return 0;} },std::vector<float> ta= {0.0});
    customer departure_updates(int station_index, float t);
    void server_updates(float t);
    void write_to_csv(std::string tandem_name);
    void logger(float t);
    void initialize_CSV(std::string file_name);
    void dump_counter_variable_memory(std::string file_name);
    int num_classes();
};
#endif