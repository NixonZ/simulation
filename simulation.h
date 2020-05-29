#include<bits/stdc++.h>
#define INF 1e4
#define endl "\n"
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

                        int find_customer = std::get<0>(counter_variable[0]);
                        
                        for( auto &x: counter_variable)
                        {
                            if (current_queue.front() == std::get<0>(x) )
                                find_customer = std::get<0>(x);
                        }
                        
                        std::get<4>(counter_variable[find_customer]) = t;

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


                        int find_customer = std::get<0>(counter_variable[0]);

                        for (auto &x : counter_variable)
                        {
                            if (current_queue.front() == std::get<0>(x))
                                find_customer = std::get<0>(x);
                        }


                        std::get<4>(counter_variable[find_customer]) = t;

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

        int find_customer = std::get<0>(counter_variable[0]);

        for (auto &x : counter_variable)
        {
            if ( customer_id == std::get<0>(x))
                find_customer = std::get<0>(x);
        }
        std::get<4>(counter_variable[find_customer]) = t;
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
        current_queue.pop();
    }
    else
        current_customer[k] = -1;

    td[k] = INF;

    if (server_status[k] == 2)
    {
        server_status[k] = -1;
    }
    else
    {
        assert(server_status[k] == 1);
        server_status[k] = 0;
        std::vector<int> disconnected_filled, connected;
        for (int i = 0; i < mxN; i++)
        {
            if (server_status[i] == 1)
                connected.push_back(i);
            else if (server_status[i] == 2)
                disconnected_filled.push_back(i);
        }

        int Q = n - disconnected_filled.size() - connected.size();

        if (Q > 0)
        {
            // Queue already popped 
            server_status[k] = 1;
            td[k] = t + DepartureTimes((t - int(t)) + int(t) % 1440);

            int find_customer = std::get<0>(counter_variable[0]);

            for (auto &x : counter_variable)
            {
                if (current_customer[k] == std::get<0>(x) )
                    find_customer = std::get<0>(x);
            }
            
            std::get<4>(counter_variable[find_customer]) = t;
        }
        else
        {
            assert(Q == 0);
            td[k] = INF;
        }
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

class system_
{
    std::vector<station> station_list;
    int number_of_station;

public:
    system_(std::vector<station> temp)
    {
        station_list = temp;
        number_of_station = temp.size();
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
};

void system_::add_customer_to_system(float t, int arriving_customer)
{
    station_list[0].add_customer_to_station(t, arriving_customer);
}

void system_::departure_updates(int station_index, float t)
{
    if (station_index == station_list.size() - 1)
    {
        // std::cout << "---------------------------------> Departure at last station" << endl;
        station_list[station_index].station::departure_updates(t);
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