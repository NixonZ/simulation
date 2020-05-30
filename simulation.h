// #include<bits/stdc++.h>
#include<vector>
#include<fstream>
#include<queue>
#include<tuple>
#include<assert.h>
#include<functional>
#include<time.h>
#include<cmath>
#include<iostream>
#include<string>
#define INF 1e7
#define endl "\n"
#define T(t) (t - int(t)) + int(t) % 1440

class station
{
    long mxN; //max Number of Servers
    std::vector<int> server_status;
    std::vector<int> current_customer;
    std::queue<int> current_queue;
    std::vector<float> td;
    int c;
    int n;
    int Na;
    std::vector< std::tuple<int,float,int,int,float,float> > counter_variable;

    /* 
    0 - Customer id
    1 - Time of arrival
    2 - Number of people in system at arrival
    3 - Number of people in queue at arrival
    4 - Service times
    5 - Departure times
    */

    using C_type = std::function<int(float t)>;
    using event_type = std::function<float(float t)>;
    C_type C;
    event_type DepartureTimes;

public:
    station(long init_mxN, C_type C_para , event_type dept_para ,float t=0,int init_n=0)
    : C(C_para),DepartureTimes(dept_para)
    {
        mxN = init_mxN;

        server_status.assign(mxN,-1);
        current_customer.assign(mxN,-1);
        td.assign(mxN,INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers
        for (int j = 0; j < std::min(n,c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = j;
        } 
        for (int i = std::min(n,c); i < n; i++)
        {
            current_queue.push(i);
        }
    }

    station(long init_mxN, C_type C_para, float init_dept, float t = 0, int init_n = 0)
        : C(C_para), 
          DepartureTimes( [init_dept](float t) -> float { return init_dept; })
    {
        mxN = init_mxN;

        server_status.assign(mxN, -1);
        current_customer.assign(mxN, -1);
        td.assign(mxN, INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers
        for (int j = 0; j < std::min(n, c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = j;
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.push(i);
        }
    }

    station(long init_mxN, int init_C, float init_dept, float t = 0, int init_n = 0)
        : C( [init_C](float t) -> int { return init_C; }),
          DepartureTimes( [init_dept](float t) -> float { return init_dept; } )
    {
        mxN = init_mxN;

        server_status.assign(mxN, -1);
        current_customer.assign(mxN, -1);
        td.assign(mxN, INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers
        for (int j = 0; j < std::min(n, c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = j;
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.push(i);
        }
    }

    station(long init_mxN, int init_C, event_type init_dept, float t = 0, int init_n = 0)   
        : C( [init_C](float t) -> int { return init_C; } ),
          DepartureTimes( init_dept )
    {
        mxN = init_mxN;

        server_status.assign(mxN, -1);
        current_customer.assign(mxN, -1);
        td.assign(mxN, INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers
        for (int j = 0; j < std::min(n, c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = j;
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.push(i);
        }
    }

    void print_station_status(float t);
    int find_min_k()
    {
        float tk = td[0];
        int k = 0;
        for (int i = 0; i < mxN; i++)
        {
            if (tk > td[i])
            {
                tk = td[i];
                k = i;
            }
        }
        return k;
    }
    float find_min_td()
    {
        return td[find_min_k()];
    }
    void server_updates(float t);
    int departure_updates(float t);
    void add_customer_to_station(float t,int customer_id);
    void write_to_csv();
    std::vector<std::tuple<int, float, int, int, float, float>> get_counter_variable()
    {
        return counter_variable;
    }
    void logger(int station_id,float t);
};

void station::server_updates(float t)
{
    int c_ = C((t - int(t)) + int(t) % 1440);
    if (c_ > this->c)
    // Server Adding
    {
        int count_disconnected = 0;
        for (auto x : server_status)
        {
            if (x == -1)
            {
                count_disconnected++;
            }
        }
        if (count_disconnected >= c_ - this->c)
        {
            int cur = c_ - this->c;
            for (int i = 0; cur > 0; i++)
            {
                if (server_status[i] == -1)
                {
                    --cur;
                    if (!current_queue.empty())
                    {
                        current_customer[i] = current_queue.front();
                        
                        for( auto &x: counter_variable)
                        {
                            if (current_queue.front() == std::get<0>(x) )
                                std::get<4>(x) = t;
                        }
                        current_queue.pop();
                        server_status[i] = 1;
                        td[i] = t + DepartureTimes((t - int(t)) + int(t) % 1440);
                    }
                    else
                    {
                        current_customer[i] = -1;
                        server_status[i] = 0;
                    }
                }
            }
        }
        else
        {
            // for(int &x :server_status)
            //   if(x==-1)
            //     x=0 ;
            for (int i = 0; i < mxN; i++)
            {
                if (server_status[i] == -1)
                {
                    if (!current_queue.empty())
                    {
                        current_customer[i] = current_queue.front();

                        for (auto &x : counter_variable)
                        {
                            if (current_queue.front() == std::get<0>(x))
                                std::get<4>(x) = t;
                        }
                        current_queue.pop();
                        server_status[i] = 1;
                        td[i] = t + DepartureTimes((t - int(t)) + int(t) % 1440);
                    }
                    else
                    {
                        current_customer[i] = -1;
                        server_status[i] = 0;
                    }
                }
            }
            int curr = c_ - this->c - count_disconnected;
            for (int i = 0; curr; i++)
            {
                if (server_status[i] == 2)
                {
                    --curr;
                    // server_status[i]= ;
                    if (!current_queue.empty())
                    {
                        server_status[i] = 1;
                    }
                    else
                    {
                        server_status[i] = 0;
                    }
                }
            }
        }
    }
    if (c_ < this->c)
    // Server removal
    {
        std::vector<int> connected_servers, connected_empty;

        for (int i = 0; i < mxN; i++)
        {
            if (this->server_status[i] == 1)
                connected_servers.push_back(i);
            if (this->server_status[i] == 0)
            {
                connected_empty.push_back(i);
            }
        }
        if (connected_servers.size() >= this->c - c_)
        {
            int servers_to_close = this->c - c_;
            for (int &x : connected_servers)
            {
                if (servers_to_close)
                {
                    server_status[x] = 2;
                    --servers_to_close;
                }
                else
                    break;
            }
        }
        else
        {            
            for (int &x : connected_servers)
                server_status[x] = 2;
            int cur = this->c - c_ - (int)connected_servers.size();
            for (int &x : connected_empty)
                if (cur)
                {
                    server_status[x] = -1;
                    --cur;
                }
        }
    }
    this->c = c_;
}

void station::add_customer_to_station(float t,int customer_id)
{
    ++n; 
    std::vector<int> empty_servers;

    for (int i = 0; i < mxN; i++)
        if (server_status[i] == 0)
            empty_servers.push_back(i);

    counter_variable.push_back( std::make_tuple(customer_id,t,n,current_queue.size(),0,0) );

    if (empty_servers.size())
    {
        server_status[empty_servers[0]] = 1;
        td[empty_servers[0]] = t + DepartureTimes((t - int(t)) + int(t) % 1440);
        current_customer[empty_servers[0]] = customer_id;

        std::get<4>(counter_variable[counter_variable.size()-1]) = t;
    }
    else
        current_queue.push(customer_id);
    ++Na;
}

int station::departure_updates(float t)
{
    int k = find_min_k();
    int tk = td[k];
    --n;

    int find_customer = std::get<0>(counter_variable[0]);

    for (auto &x : counter_variable)
    {
        if (current_customer[k] == std::get<0>(x) )
            find_customer = std::get<0>(x);
    }

    std::get<5>(counter_variable[find_customer]) = t;

    int customer_id = current_customer[k];

    if (!current_queue.empty() && server_status[k] != 2)
    {
        assert(current_queue.front() >= 0);

        current_customer[k] = current_queue.front();

        assert(server_status[k] == 1);
        server_status[k] = 0;

        current_queue.pop();
        server_status[k] = 1;
        td[k] = t + DepartureTimes((t - int(t)) + int(t) % 1440);

        for (auto &x : counter_variable)
        {
            if (current_customer[k] == std::get<0>(x))
                std::get<4>(x) = t;
        }
    }
    else if ( current_queue.empty() && server_status[k]!=2 )
    {
        td[k] = INF;
        server_status[k] = 0;
        current_customer[k] = -1;
    }
    else
    {
        current_customer[k] = -1;
        server_status[k] = -1;
        td[k] = INF;
    }

    return customer_id;
}

void station::print_station_status(float t)
{
    std::cout << "----System Status----" << endl;
    std::cout << "Present time:" << t << endl;
    std::cout << "Number of People Present:" << n << endl;
    std::cout << "Cummulative Number of Arrivals by t:" << Na << endl;
    std::cout << "Number of Servers:" << c << endl;

    std::cout << "Server List:"
         << "[";
    for (int &x : server_status)
        std::cout << x << ',';
    std::cout << "\b]" << endl;

    std::cout << "Current Customers:"
              << "[";
    for (int &x : current_customer)
        std::cout << x << ',';
    std::cout << "\b]" << endl;

    std::cout << "Counter Variable:"
              << "[";
    for (auto &x: counter_variable)
    {
        std::cout<<"{";
        std::cout << std::get<0>(x) << ',';
        std::cout << std::get<1>(x) << ',';
        std::cout << std::get<2>(x) << ',';
        std::cout << std::get<3>(x) << ',';
        std::cout << std::get<4>(x) << ',';
        std::cout << std::get<5>(x) ;
        std::cout<<"};";
    }
    std::cout << "\b]" << endl;

    std::cout << "Departure times:"
         << "[";
    for (float &x : td)
        std::cout << x << ',';
    std::cout << "\b]" << endl;

    std::cout << "Queue list: [";
    std::vector<int> v;
    while (!current_queue.empty())
    {
        int x = current_queue.front();
        v.push_back(x);
        current_queue.pop();
    }
    for (auto &x : v)
    {
        std::cout << x << ',';
        current_queue.push(x);
    }
    if(!current_queue.empty())
        std::cout << "\b]" << endl;
    else
        std::cout <<']' <<endl;
}

void station::logger(int station_id,float t)
{
    std::ofstream logi;
    if ( t == 0)
    {
        logi.open("./logs/log_" + std::to_string(station_id) + ".txt", std::ofstream::out );
    }
    else
        logi.open("./logs/log_" + std::to_string(station_id) + ".txt", std::ios_base::app);

    logi << "----System Status----" << endl;
    logi << "Present time:" << t << endl;
    logi << "Number of People Present:" << n << endl;
    logi << "Cummulative Number of Arrivals by t:" << Na << endl;
    logi << "Number of Servers:" << c << endl;

    logi << "Server List:"
         << "[";
    for (int &x : server_status)
        logi << x << ',';
    logi << "]" << endl;

    logi << "Current Customers:"
         << "[";
    for (int &x : current_customer)
        logi << x << ',';
    logi << "]" << endl;

    logi<< "Counter Variable:"
          << "[";
    for (auto &x : counter_variable)
    {
        logi << "{";
        logi << std::get<0>(x) << ',';
        logi << std::get<1>(x) << ',';
        logi << std::get<2>(x) << ',';
        logi << std::get<3>(x) << ',';
        logi << std::get<4>(x) << ',';
        logi << std::get<5>(x);
        logi << "};";
    }
    logi << "]" << endl;

    logi << "Departure times:"
              << "[";
    for (float &x : td)
        logi << x << ',';
    logi << "]" << endl;

    logi << "Queue list: [";
    std::vector<int> v;
    while (!current_queue.empty())
    {
        int x = current_queue.front();
        v.push_back(x);
        current_queue.pop();
    }
    for (auto &x : v)
    {
        logi << x << ',';
        current_queue.push(x);
    }
    logi << ']' << endl;
    logi.close();
}

void station::write_to_csv()
{
    std::ofstream data;
    data.open("data.csv", std::ofstream::out);
    data << "Customer,Time of arrival,Number of people at arrival,Number of people currently in queue at arrival,Which 10 min interval in day,Which day in week,Time of start of service,Departure time,Wait time,\n";
    for(auto &x: counter_variable)
    {
        data << std::get<0>(x) << ","
             << std::get<1>(x) << ","
             << std::get<2>(x) << ","
             << std::get<3>(x) << ","
             << ((int(std::get<1>(x))) % 1440) / 10 << ','
             << int(std::get<1>(x) / 1440) % 7 << ','
             << std::get<4>(x) << ","
             << std::get<5>(x) << ","
             << (std::get<4>(x) - std::get<1>(x)) << ","
             << "\n";
    }
}



class system_
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
    system_(std::vector<station> temp)
    {
        station_list = temp;
        number_of_station = temp.size();
        N = 0;
    }

    std::tuple<int, float> find_least_dep_time()
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
    void print_system_status(float t);
    void add_customer_to_system(float t, int arriving_customer);
    void departure_updates(int station_index, float t);
    void server_updates(float t);
    void write_to_csv();
    void logger(float t);
};

void system_::add_customer_to_system(float t, int arriving_customer)
{
    N++;
    station_list[0].add_customer_to_station(t, arriving_customer);
    system_counter_variable.push_back( std::make_tuple(arriving_customer,t,N,0,0) );
}

void system_::departure_updates(int station_index, float t)
{
    if (station_index == station_list.size() - 1)
    {
        // std::cout << "---------------------------------> Departure at last station" << endl;
        N--;

        int departing_customer = station_list[station_index].station::departure_updates(t);

        float service_time = 0;

        for(auto &x: station_list[station_index].get_counter_variable() )
        {
            if(departing_customer == std::get<0>(x))
                service_time = std::get<4>(x);
        }

        for(auto &x: system_counter_variable)
        {
            if( departing_customer == std::get<0>(x))
            {
                std::get<3>(x) = service_time;
                std::get<4>(x) = t;
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

void system_::print_system_status(float t)
{
    int l = 0;
    for (auto &x : station_list)
    {
        std::cout << "#### Station No." << l++ << "####" << endl;
        x.print_station_status(t);
    }
}

void system_::server_updates(float t)
{
    for (auto &x : station_list)
    {
        x.server_updates(t);
    }
}

void system_::logger(float t)
{
    int l = 0;
    for (auto &x : station_list)
        x.logger(l++,t);
}

void system_::write_to_csv()
{
    std::ofstream data;

    data.open("data_system.csv", std::ofstream::out);
    data << "Customer,Time of arrival,Number of people at arrival,Which 10 min interval in day,Which day in week,Time of start of service,Departure time,Wait time,\n";

    int last_customer_in_last_station = 0;
    for(auto &x: station_list[number_of_station-1].get_counter_variable() )
    {
        if (std::get<0>(x)>last_customer_in_last_station)
            last_customer_in_last_station = std::get<0>(x);
    }

    for (auto &x : system_counter_variable)
    {
        data << std::get<0>(x) << ","
             << std::get<1>(x) << ","
             << std::get<2>(x) << ","
             << ((int(std::get<1>(x))) % 1440) / 10 << ','
             << int(std::get<1>(x) / 1440) % 7 << ','
             << std::get<3>(x) << ","
             << std::get<4>(x) << ","
             << (std::get<3>(x) - std::get<1>(x)) << ","
             << "\n";
        if ( std::get<0>(x) == last_customer_in_last_station)
            break;
    }
}



