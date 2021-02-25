#include "tandem.h"

void tandem::add_customer_to_system(float t, customer curr_customer, bool keep_virtual, event_type_list arrival_processes ,std::vector<float> ta)
{
    N++;

    float virtual_wait_time = -1;
    // Generate virtual Waiting time by running a copy of the simulation
    if(keep_virtual)
    {
        float least_dep_time = 0;
        int least_station_index = 0;
        tandem virtual_wait( *this );
        int t_ = t;
        int arriving_customer = curr_customer[1];

        virtual_wait.add_customer_to_system(t_,curr_customer,false);
        arriving_customer++;
        ta[curr_customer[0]] = t_ + arrival_processes[curr_customer[0]](t_) ;

        customer dept_cust;

        int priority;
        
        while (true)
        {
            std::tie(least_station_index, least_dep_time) = virtual_wait.find_least_dep_time();
            t_ = std::min(least_dep_time, *std::min_element(ta.begin(),ta.end()) );

            virtual_wait.server_updates(t_);

            if (t_ == *std::min_element(ta.begin(),ta.end()))
            {
                //arrival happening
                priority = std::distance(ta.begin(), std::min_element(ta.begin(),ta.end()) );
                virtual_wait.add_customer_to_system(t_, {priority,arriving_customer},false);
                arriving_customer++;
                ta[priority] = t_ + arrival_processes[priority](t_) ;
            }
            else if(least_station_index == number_of_station-1)
            {
                dept_cust = virtual_wait.departure_updates(least_station_index,t);
                if(dept_cust == curr_customer)
                    break;
            }
            else
                virtual_wait.departure_updates(least_station_index,t);
        }
        virtual_wait_time = t_ - t;
    }
    // Comparision of patience times and decide whether to add customer
    float patience_time = PatienceTimes[curr_customer[0]](t);

    bool add = (patience_time>virtual_wait_time);

    std::vector<std::tuple<int,int,int>> temp;
    for(auto& station:station_list)
    {
        temp.push_back(station.access_system_state(t));
    }

    if(add)
    {
        station_list[0].add_customer_to_station(t, curr_customer,false);
        system_counter_variable.push_back(
            std::make_tuple(curr_customer, t, N,temp, 0, 0)
        );
    }
    else
    {
        system_counter_variable.push_back(
            std::make_tuple(curr_customer, t, N,temp, -1, -1)
        );
    }
    
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

customer tandem::departure_updates(int station_index, float t)
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
                std::get<4>(system_counter_variable[i]) = service_time;
                std::get<5>(system_counter_variable[i]) = t;
                break;
            }
        }
        return departing_customer;
        // do only departure updates for last station
    }
    else
    {
        // std::cout << "---------------------------------> Departure at station :" << station_index << endl;
        station_list[station_index + 1].add_customer_to_station(t, station_list[station_index].departure_updates(t));
        return {1,1};
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

    data.open( tandem_name + ".csv", std::ofstream::out|std::ofstream::trunc);
    data << "Customer ID,Priority Level,Time of arrival,Number of people at arrival,Which 10 min interval in day,Which day in week,";
    for(int i=0;i<this->number_of_station;i++)
    {
        data << "Number of people at station " <<i <<" at arrival,";
        data << "Number of people in queue at station " <<i <<" at arrival,";
        data << "Number of active servers in station " <<i <<',';
    }
    data <<"Time of start of service,Departure time,Wait time,\n";

    for (auto &x : system_counter_variable)
    {
        if(std::get<5>(x) == 0)
            continue;
        data << std::get<0>(x)[1] <<","
             << std::get<0>(x)[0] <<","
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
    }
    data.close();
}

void tandem::initialize_CSV(std::string tandem_name = "data_system")
{
    std::ofstream data;

    data.open( tandem_name + ".csv", std::ofstream::out|std::ofstream::trunc);
    data << "Customer ID,Priority Level,Time of arrival,Number of people at arrival,Which 10 min interval in day,Which day in week,";
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
    
    for (auto &x : system_counter_variable)
    {
        if(std::get<5>(x) == 0)
            continue;
        data << std::get<0>(x)[1] <<","
             << std::get<0>(x)[0] <<","
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
    }
    
    std::vector<tandem_data> system_counter_variable_temp;
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

int tandem::num_classes()
{
    return this->station_list[0].num_classes();

}