#include "station.h"

/** @brief Finds the server index which has the minimal departure time.
 *
 *  @details
 *   Finds the server index with departure time with a simple loop.
 *   Completed in O(mxN).
 * 
 *  @return     Index of the server with minimum departure time.
 */
int station::find_min_k()
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
/** @brief Finds the minimum departure time of all the servers working.
 *
 *  @details
 *   Uses station::find_min_k .
 * 
 *  @see station::find_min_k
 *  @return     Minimum departure time.
 */
float station::find_min_td()
{
    return td[find_min_k()];
}

/// @brief Makes server updates.
///
///  @param t  The time variable.
///
///  @details
///  This function is required when the servers are dynamic. This\n
///  code manages the opening and closing of servers.Directly modifies station::server_status\n
///  Whenever the number of active server changes, the following rules govern the opening and closing:\n
///  1. Server closure:       
///    1. The servers currently working(1) are set to 2 ( i.e. will be closed(-1) as soon    
///        as server becomes idle(0) again).
///    2. If more server needs to be closed after 1. then the idle(0) servers are closed(-1).\n
///  2. Server opening:
///    1. The inactive(-1) servers are set to active again and the front the queue     
///       is pushed to the newly openend servers and their departure times are generated.
///    2. If still more servers are needed, the servers with status 2 are opened(1).
///
///  @note     Must be called every time the number of servers (station::c) change value as is defined by the server update function (station::C).
///  @warning  Assumes that the server update function (station::C) is well-defined.
///  
void station::server_updates(float t)
{
    int c_ = C(T(t));
    if (c_ > this->c)
    // Server Adding
    {
        int count_disconnected = 0;
        for (auto x : server_status) // O(mxN) redundant can be counted on runtime
        {
            if (x == -1)
            {
                count_disconnected++;
            }
        }
        if (count_disconnected >= c_ - this->c)
        {
            int cur = c_ - this->c; // Number of servers to open
            for (int i = 0; cur > 0; i++) 
            {
                if (server_status[i] == -1)
                {
                    --cur;
                    // Open server and add customer at front of the Queue.
                    if (!current_queue.empty())
                    {
                        current_customer[i] = (*current_queue.begin()); // Front of queue put to newly opened server

                        for(int i=counter_variable.size()-1;i>=0;i--)
                        {
                            if ((*current_queue.begin()) == std::get<0>(counter_variable[i]))
                            {
                                std::get<4>(counter_variable[i]) = t;
                                break;
                            }
                        }

                        current_queue.erase(current_queue.begin()) ;

                        server_status[i] = 1;
                        td[i] = t + DepartureTimes[current_customer[i][0]]((t - int(t)) + int(t) % 1440);
                    }
                    else
                    {
                        current_customer[i] = {-1,-1};
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
                        current_customer[i] = (*current_queue.begin());

                        for(int i=counter_variable.size()-1;i>=0;i--)
                        {
                            if ((*current_queue.begin()) == std::get<0>(counter_variable[i]))
                            {
                                std::get<4>(counter_variable[i]) = t;
                                break;
                            }
                        }

                        current_queue.erase(current_queue.begin()) ;
                        server_status[i] = 1;
                        td[i] = t + DepartureTimes[current_customer[i][0]]((t - int(t)) + int(t) % 1440);
                    }
                    else
                    {
                        current_customer[i] = {-1,-1};
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

/// @brief Add a single customer to the front of the system.
///  @param t  The time variable.
///  @param curr_customer Prioirty and ID of the customer which is to be added. Maintain a counter for the customers.\n
///                     Call this function whenever the time variable reaches an arrival time.
///  @param arrival_process A list of functions which generate arrival times for each prioirty.
///  @param ta The next arrival times for each prioirity level.
///  @param keep_virtual Whether to calculate virual Waiting Time or not. Default false. Keep True when Patience Times are used.
///  @details
///  Increases the number of customer present in the system. Creates a new entry in station::counter_variable for the\n
///  incoming customer. The customer is sent to the first empty server or in case no empty server is found,\n
///  to the back of the queue. (station::current_queue). Can Calculate the virtual times if patience times are given and customer abandonment can be implemented.
///
///  @note     Must be called when time variable reahes next time of arrival.
///  @warning  Assumes customer id given are sequential integers.
///
///  Example:
///  @code
///     station temp; //suitably defined
///     int discrete_events = 0;
///     int arriving_customer = 0;
///     float t = 0;
///     float ta = Ts_generator(t);
///     while (discrete_events < 50)
///     {
///         t = std::min(temp.find_min_td(), ta);
///         temp.server_updates(t);
///         if (t == ta)
///         {
///             //arrival happening
///            priority = priority_gen(temp.num_classes());
///            temp.add_customer_to_station(t, {priority,arriving_customer});
///            arriving_customer++;
///            ta = t + exponentialrv(0.1);
///         }
///         else
///             temp.departure_updates(t);
///         temp.print_station_status(t);
///         discrete_events++;
///     }
///  @endcode
void station::add_customer_to_station(float t, customer curr_customer, bool keep_virtual = false, event_type_list arrival_processes = { [](float t)-> float{return 0;} },std::vector<float> ta = {0.0})
{
    
    float virtual_wait_time = -1;
    // Generate virtual Waiting time by running a copy of the simulation
    if(keep_virtual)
    {
        station virtual_wait( *this );
        int t_ = t;
        int arriving_customer = curr_customer[1];

        virtual_wait.add_customer_to_station(t_,curr_customer,false);
        arriving_customer++;
        ta[curr_customer[0]] = t_ + arrival_processes[curr_customer[0]](t_) ;

        customer dept_cust;

        int priority;
        
        while (true)
        {
            t_ = std::min( virtual_wait.find_min_td(),  *std::min_element(ta.begin(),ta.end()) );

            virtual_wait.server_updates(t_);

            if (t_ == *std::min_element(ta.begin(),ta.end()))
            {
                //arrival happening
                priority = std::distance(ta.begin(), std::min_element(ta.begin(),ta.end()) );
                virtual_wait.add_customer_to_station(t_, {priority,arriving_customer},false);
                arriving_customer++;
                ta[priority] = t_ + arrival_processes[priority](t_) ;
            }
            else
                dept_cust = virtual_wait.departure_updates(t_);
            
            if(dept_cust == curr_customer)
                break;
        }
        virtual_wait_time = t_ - t;
    }
    // Comparision of patience times and decide whether to add customer
    float patience_time = PatienceTimes[curr_customer[0]](t);
    
    bool add = (patience_time>virtual_wait_time);

    if(add)
    {
        ++n;
        std::vector<int> empty_servers;

        for (int i = 0; i < mxN; i++) // O(mxN)
            if (server_status[i] == 0)
                empty_servers.push_back(i);

        counter_variable.push_back(std::make_tuple(curr_customer, t, n, current_queue.size(), 0, 0));

        if (empty_servers.size())
        {
            assert(current_queue.size()==0);
            server_status[empty_servers[0]] = 1;
            td[empty_servers[0]] = t + DepartureTimes[curr_customer[0]]((t - int(t)) + int(t) % 1440);
            current_customer[empty_servers[0]] = curr_customer;

            std::get<4>(counter_variable[counter_variable.size() - 1]) = t;
        }
        else
            current_queue.insert(curr_customer);
        ++Na;
    }
    else
    {
        counter_variable.push_back(std::make_tuple(curr_customer, t, n, current_queue.size(), -1, -1));
    }
    
}

/// @brief Gives back the station::counter_variable.
///
/// @return station::counter_variable.
///
///  Example: 
///  @code 
///     for(auto&x: MM1.get_counter_variable())
///             {
///                 if( std::get<0>(x) == customer_departing )
///                     X = std::get<4>(x) - std::get<1>(x);
///             }
///  @endcode
std::vector<customer_data> station::get_counter_variable()
{
    return counter_variable;
}

/// @brief Makes departure updates.
///
///  @param t  The time variable.
///
///  @details
///  Removes the customer whose service is completed at time t. Updates the entry in station::counter_variable for the\n
///  outgoing customer. A new customer is added to the server or else server is closed if the server status was 2 or\n
///  the server is set to idle if the system is empty.
///
///  @return  The customer id of the customer departing.
///  @note     Must be called when time variable reahes next departure time.
///
///  Example:
///  @code
///     station temp; //suitably defined
///     int discrete_events = 0;
///     int arriving_customer = 0;
///     float t = 0;
///     float ta = Ts_generator(t);
///     while (discrete_events < 50)
///     {
///         t = std::min(temp.find_min_td(), ta); //station::find_mid_td
///         temp.server_updates(t);
///         if (t == ta)
///         {
///             //arrival happening
///            priority = priority_gen(temp.num_classes());
///            temp.add_customer_to_station(t, {priority,arriving_customer});
///            arriving_customer++;
///            ta = t + exponentialrv(0.1);
///         }
///         else
///             temp.departure_updates(t);
///         temp.print_station_status(t);
///         discrete_events++;
///     }
///  @endcode
customer station::departure_updates(float t)
{
    int k = find_min_k(); //O(mxN)
    int tk = td[k];
    --n;

    // counter variable is not sorted in case of tandem and graph.
    customer find_customer = std::get<0>(counter_variable[0]);

    int index = 0;
    for (int i = counter_variable.size()-1;i>=0;i--)
    {
        if (current_customer[k] == std::get<0>(counter_variable[i]))
        {
            index = i;
            find_customer = std::get<0>(counter_variable[i]);
            break;
        }
    }
    

    std::get<5>(counter_variable[index]) = t;

    customer dept_customer = current_customer[k];

    if (!current_queue.empty() && server_status[k] != 2)
    {

        current_customer[k] = (*current_queue.begin()); // O(logn) time insertion and access via balanced binary search tree

        assert(server_status[k] == 1);
        server_status[k] = 0;

        current_queue.erase(current_queue.begin()) ;
        server_status[k] = 1;

        td[k] = t + DepartureTimes[current_customer[k][0]]((t - int(t)) + int(t) % 1440);

        // insert service time in counter variable
        for(int i=counter_variable.size()-1;i>=0;i--)
        {
            if (current_customer[k] == std::get<0>(counter_variable[i]))
            {
                std::get<4>(counter_variable[i]) = t;
                break;
            }
        }
    }

    else if (current_queue.empty() && server_status[k] != 2)
    {
        td[k] = INF;
        server_status[k] = 0;
        current_customer[k] = {-1,-1};
    }
    else
    {
        current_customer[k] = {-1,-1};
        server_status[k] = -1;
        td[k] = INF;
    }

    return dept_customer;
}

/// @brief Beautifully outputs station status to the console.
/// @param t The time variable
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
    for (customer &x : current_customer)
        std::cout <<'{'<< x[1] << '|'<<x[0] <<'}' <<',';
    std::cout << "\b]" << endl;

    std::cout << "Counter Variable:"
              << "[";
    for (auto &x : counter_variable)
    {
        std::cout << "{";
        std::cout <<'{'<< std::get<0>(x)[1] << '|' <<std::get<0>(x)[0] <<'}' <<',';
        std::cout << std::get<1>(x) << ',';
        std::cout << std::get<2>(x) << ',';
        std::cout << std::get<3>(x) << ',';
        std::cout << std::get<4>(x) << ',';
        std::cout << std::get<5>(x);
        std::cout << "};";
    }
    std::cout << "\b]" << endl;

    std::cout << "Departure times:"
              << "[";
    for (float &x : td)
        std::cout << x << ',';
    std::cout << "\b]" << endl;

    std::cout << "Queue list: [";

    for (customer x : current_queue)
        std::cout <<'{'<< x[1] << '|'<<x[0] <<'}' <<',';
    
    if (!current_queue.empty())
        std::cout << "\b]" << endl;
    else
        std::cout << ']' << endl;
}

/// @brief Beautifully outputs station status a file in folder logs.
/// @param station_id Used when the stations are sequential. Used mostly in tandem class.
/// @param t Time variable
void station::logger(int station_id, float t)
{
    std::ofstream logi;
    if (t == 0)
    {
        logi.open("./logs/log_" + std::to_string(station_id) + ".txt", std::ofstream::out|std::ofstream::trunc);
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
    for (customer &x : current_customer)
        logi <<'{'<< x[1] << '|'<<x[0] <<'}' <<',';
    logi << "]" << endl;

    logi << "Counter Variable:"
         << "[";
    for (auto &x : counter_variable)
    {
        logi << "{";
        logi <<'{'<< std::get<0>(x)[1] << '|' <<std::get<0>(x)[0] <<'}' <<',';
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
    for (customer x : current_queue)
        logi <<'{'<< x[1] << '|'<<x[0] <<'}' <<',';
    logi << ']' << endl;
    logi.close();
}

/// @brief Beautifully outputs station status a file in folder logs.
/// @param station_id Used when a uninque name is to be given to the text file.
/// @param t Time variable.
void station::logger(std::string station_id, float t)
{
    std::ofstream logi;
    if (t == 0)
    {
        logi.open("./logs/log_" + station_id + ".txt", std::ofstream::out);
    }
    else
        logi.open("./logs/log_" + station_id + ".txt", std::ios_base::app);

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
    for (customer &x : current_customer)
        logi <<'{'<< x[1] << '|'<<x[0] <<'}' <<',';
    logi << "]" << endl;

    logi << "Counter Variable:"
         << "[";
    for (auto &x : counter_variable)
    {
        logi << "{";
        logi <<'{'<< std::get<0>(x)[1] << '|' <<std::get<0>(x)[0] <<'}' <<',';
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
    for (customer x : current_queue)
        logi <<'{'<< x[1] << '|'<<x[0] <<'}' <<',';
    logi << ']' << endl;
    logi.close();
}

/// @brief Writes the station::counter_variable to a csv file.
/// @param file_name The name of the csv file to be created.
void station::write_to_csv(std::string file_name)
{
    std::ofstream data;
    data.open( file_name + ".csv", std::ofstream::out|std::ofstream::trunc);
    data << "Customer ID,Prority Level,Time of arrival,Number of people at arrival,Number of people currently in queue at arrival,Which 10 min interval in day,Which day in week,Time of start of service,Departure time,Wait time,\n";
    for (auto &x : counter_variable)
    {
        if(std::get<5>(x) == 0)
            continue;
        data << std::fixed << std::setprecision(precision);
        data << std::get<0>(x)[1] <<","
             << std::get<0>(x)[0] <<","
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
    data.close();
}

/// @brief Resets the queue.
/// @details Resets station::server_status,station::current_customer,station::current_queue and station::td.\n
///          Also clears station::counter_variable. Use when a queue is to be used afresh.
/// @note Currently not working.
void station::reset_queue(float t)
{
    n=0;
    Na = n;
    server_status.clear();
    server_status.assign(mxN, -1);
    current_customer.clear();
    current_customer.assign(mxN,{-1,-1});
    current_queue.clear();
    td.clear();
    td.assign(mxN, INF);
    counter_variable.clear();
    this->server_updates(T(t));
}

/// @brief Gives the minimum residual time i.e. time left for next departure.
/// @details The minimum residual time is directlty related to the waiting time of a customer.
/// @param t Time variable
float station::minimum_residual_time(float t)
{
    return this->find_min_td()-t;
}

std::tuple<int,int,int> station::access_system_state(float t)
{
    return std::make_tuple(  n, current_queue.size(), C(T(t)) );
}

void station::initialize_CSV(std::string file_name)
{
    std::ofstream data;
    data.open( file_name + ".csv", std::ofstream::out|std::ofstream::trunc);
    data << "Customer ID,Priority Level,Time of arrival,Number of people at arrival,Number of people currently in queue at arrival,Which 10 min interval in day,Which day in week,Time of start of service,Departure time,Wait time,\n";
    data.close();
}

void station::dump_counter_variable_memory(std::string file_name)
{
    std::ofstream data;
    data.open( file_name + ".csv", std::ofstream::app);

    for (auto &x : counter_variable)
    {
        if(std::get<5>(x) == 0)
            continue;
        data << std::fixed << std::setprecision(precision);
        data << std::get<0>(x)[1] <<","
             << std::get<0>(x)[0] <<","
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
    data.close();
    std::vector< customer_data > counter_variable_temp;
    counter_variable_temp.assign(0,{});
    for(int i = 0;i<this->counter_variable.size();i++)
    {
        if(std::get<5>(this->counter_variable[i])==0)
            counter_variable_temp.push_back(this->counter_variable[i]);
    }
    this->counter_variable.clear();
    this->counter_variable = counter_variable_temp;
}

void station::dump_counter_variable_memory()
{
    std::vector< customer_data > counter_variable_temp;
    counter_variable_temp.assign(0,{});
    for(int i = 0;i<this->counter_variable.size();i++)
    {
        if(std::get<5>(this->counter_variable[i])==0)
            counter_variable_temp.push_back(this->counter_variable[i]);
        
    }
    this->counter_variable.clear();
    this->counter_variable = counter_variable_temp;
}

/// @brief Return number of classes
///
///  @return Number of priority levels
///
///  
int station::num_classes()
{
    return this->num_class;
}

/// @brief Reads data from a CSV file.
/// @details Will read a particular column from the CSV containing observed values of random variables.
/// @param filename The path of the CSV file from which data is to be read.
/// @param index Index is the column number to be read.\n
///              Default 1. Column 0 is assumed to be the uniform random numbers generated.
std::vector<float> read_csv(std::string filename,int index = 1)
{
    std::ifstream fin;
    fin.open(filename,std::ifstream::in);
    std::string temp,line,word;
    std::vector<std::string> row;
    std::vector<float> data;
    while(!fin.eof())
    {
        row.clear();
        std::getline(fin,line,'\n');
        std::stringstream s(line);
        while (std::getline(s, word, ',')) 
        { 
            row.push_back(word); 
        } 
        try
        {
            data.push_back( std::stof(row[index]) );
        }
        catch(const std::exception& e)
        {
            // std::cerr << e.what() << '\n';
            continue;
        }
    }
    fin.close();
    return data;
}

/// @brief Computes the vector of functions of event types.
/// @param fncs The vector of functions which is to be copmuted. f: float -> float
/// @return A vector of float
std::vector<float> call_event_type_list(event_type_list fncs)
{
    std::vector<float> temp;
    for(auto x:fncs)
    {
        temp.push_back(x(0));
    }
    return temp;
}