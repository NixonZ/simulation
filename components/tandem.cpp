#include "tandem.h"

tandem::tandem(std::vector<station> temp)
{
    station_list = temp;
    number_of_station = temp.size();
    N = 0;
}

void tandem::add_customer_to_system(float t, int arriving_customer)
{
    N++;
    station_list[0].add_customer_to_station(t, arriving_customer);
    std::vector<std::tuple<int,int,int>> temp;
    for(auto& station:station_list)
    {
        temp.push_back(station.access_system_state(t));
    }
    system_counter_variable.push_back(
        std::make_tuple(arriving_customer, t, N,temp, 0, 0)
        );
}

//O(mxN*numberofstation)
std::tuple<int, float> tandem::find_least_dep_time()
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

void tandem::departure_updates(int station_index, float t)
{
    if (station_index == station_list.size() - 1)
    {
        // std::cout << "---------------------------------> Departure at last station" << endl;
        N--;

        int departing_customer = station_list[station_index].station::departure_updates(t);

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
                std::get<4>(system_counter_variable[i]) = service_time;
                std::get<5>(system_counter_variable[i]) = t;
                break;
            }
        }
        // do only departure updates for last station
    }
    else
    {
        // std::cout << "---------------------------------> Departure at station :" << station_index << endl;
        station_list[station_index + 1].add_customer_to_station(t, station_list[station_index].departure_updates(t));

        // do departure updates for station_index
        // do arrival updates for station_index+1
    }
}

void tandem::print_system_status(float t)
{
    int l = 0;
    for (auto &x : station_list)
    {
        std::cout << "#### Station No." << l++ << "####" << endl;
        x.print_station_status(t);
    }
}

void tandem::server_updates(float t)
{
    for (auto &x : station_list)
    {
        x.server_updates(t);
    }
}

void tandem::logger(float t)
{
    int l = 0;
    for (auto &x : station_list)
        x.logger(l++, t);
}

void tandem::write_to_csv(std::string tandem_name = "data_system")
{
    std::ofstream data;

    data.open( tandem_name + ".csv", std::ofstream::out);
    data << "Customer,Time of arrival,Number of people at arrival,Which 10 min interval in day,Which day in week,";
    for(int i=0;i<this->number_of_station;i++)
    {
        data << "Number of people at station " <<i <<" at arrival,";
        data << "Number of people in queue at station " <<i <<" at arrival,";
        data << "Number of active servers in station " <<i <<',';
    }
    data <<"Time of start of service,Departure time,Wait time,\n";

    int last_customer_in_last_station = 0;

    for (int i = station_list[number_of_station - 1].get_counter_variable().size()-1;i>=0;i++)
    {
        if (std::get<5>(station_list[number_of_station - 1].get_counter_variable()[i]) > 0 )
        {
            last_customer_in_last_station = std::get<0>(station_list[number_of_station - 1].get_counter_variable()[i]);
            break;
        }
    }

    for (auto &x : system_counter_variable)
    {
        if(std::get<5>(x) == 0)
            break;
        data << std::get<0>(x) << ","
             << std::get<1>(x) << ","
             << std::get<2>(x) << ","
             << ((int(std::get<1>(x))) % 1440) / 10 << ','
             << int(std::get<1>(x) / 1440) % 7 << ',';

        for(auto &station_info:std::get<3>(x))
        {
            data << std::get<0>(station_info) <<',';
            data << std::get<1>(station_info) <<',';
            data << std::get<2>(station_info) <<',';
        }


        data << std::get<4>(x) << ","
             << std::get<5>(x) << ","
             << (std::get<4>(x) - std::get<1>(x)) << ","
             << "\n";
        if (std::get<0>(x) == last_customer_in_last_station)
            break;
    }
    data.close();
}

void tandem::initialize_CSV(std::string tandem_name = "data_system")
{
    std::ofstream data;

    data.open( tandem_name + ".csv", std::ofstream::out);
    data << "Customer,Time of arrival,Number of people at arrival,Which 10 min interval in day,Which day in week,";
    for(int i=0;i<this->number_of_station;i++)
    {
        data << "Number of people at station " <<i <<" at arrival,";
        data << "Number of people in queue at station " <<i <<" at arrival,";
        data << "Number of active servers in station " <<i <<',';
    }
    data <<"Time of start of service,Departure time,Wait time,\n";
    data.close();
}

void tandem::dump_counter_variable_memory(std::string tandem_name = "data_system")
{
    std::ofstream data;

    data.open( tandem_name + ".csv", std::ofstream::app);
    

    int last_customer_in_last_station = 0;
    for (auto &x : station_list[number_of_station - 1].get_counter_variable())
    {
        if (std::get<0>(x) > last_customer_in_last_station)
            last_customer_in_last_station = std::get<0>(x);
    }

    for (auto &x : system_counter_variable)
    {
        if(std::get<5>(x) == 0)
            break;
        data << std::get<0>(x) << ","
             << std::get<1>(x) << ","
             << std::get<2>(x) << ","
             << ((int(std::get<1>(x))) % 1440) / 10 << ','
             << int(std::get<1>(x) / 1440) % 7 << ',';

        for(auto &station_info:std::get<3>(x))
        {
            data << std::get<0>(station_info) <<',';
            data << std::get<1>(station_info) <<',';
            data << std::get<2>(station_info) <<',';
        }


        data << std::get<4>(x) << ","
             << std::get<5>(x) << ","
             << (std::get<4>(x) - std::get<1>(x)) << ","
             << "\n";
        if (std::get<0>(x) == last_customer_in_last_station)
            break;
    }
    
    std::vector<std::tuple<int, float, int,std::vector<std::tuple<int,int,int>>, float, float>> system_counter_variable_temp;
    system_counter_variable_temp.assign(0,{});
    for(int i = 0;i<this->system_counter_variable.size();i++)
    {
        if(std::get<5>(this->system_counter_variable[i])==0)
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