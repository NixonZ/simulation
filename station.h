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
    std::vector<std::array<float, 3>> A;
    std::vector<float> D;
    std::vector<float> S;

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
        A.assign(INF,{0,0,0});
        D.assign(INF,0);
        S.assign(INF,0);
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
        A.assign(INF, {0, 0, 0});
        D.assign(INF, 0);
        S.assign(INF, 0);
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
        A.assign(INF, {0, 0, 0});
        D.assign(INF, 0);
        S.assign(INF, 0);
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
        A.assign(INF, {0, 0, 0});
        D.assign(INF, 0);
        S.assign(INF, 0);
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
    void departure_updates(float t);
    void add_customer_to_station(float t);
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
                        S[current_queue.front()] = t;
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
                        S[current_queue.front()] = t;
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

void station::add_customer_to_station(float t)
{
    ++n; 
    std::vector<int> empty_servers;

    for (int i = 0; i < mxN; i++)
        if (server_status[i] == 0)
            empty_servers.push_back(i);

    if (empty_servers.size())
    {
        server_status[empty_servers[0]] = 1;
        td[empty_servers[0]] = t + DepartureTimes((t - int(t)) + int(t) % 1440);
        current_customer[empty_servers[0]] = Na;
        S[Na] = t;
    }
    else
        current_queue.push(Na);
    A[Na][0] = t;
    A[Na][1] = n;
    A[Na][2] = current_queue.size();
    ++Na;
}

void station::departure_updates(float t)
{
    int k = find_min_k();
    int tk = td[k];
    --n;
    D[current_customer[k]] = t;
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
            server_status[k] = 1;
            td[k] = t + DepartureTimes((t - int(t)) + int(t) % 1440);
            S[current_customer[k]] = t;
        }
        else
        {
            assert(Q == 0);
            td[k] = INF;
        }
    }
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


