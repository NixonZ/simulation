#include "./../includes.h"
#ifndef STATION_H
#define STATION_H
/**
 * Station class implements a single Queueing System.
 * 
 */
class station
{
    long mxN; /*!< max Number of Servers */
    std::vector<int> server_status; /*!< Present status of the server \n
                                         -1 : Server is closed \n
                                          2 : Server is working but will be closed on service completion \n
                                          0 : Server empty \n
                                          1 : Server.  */
    std::vector<customer> current_customer; /*!< The current customers present in the server.*/
    std::set<customer> current_queue; /*!< The current queue of the system.*/
    std::vector<float> td; /*!< Departure times.*/
    int c; /*!< Number of servers.*/
    int n; /*!< Number of customers in system*/
    int Na; /*!< Cummulative number of arrivals.*/
    std::vector< customer_data > counter_variable; /*!< 
    0 - Customer
    1 - Time of arrival
    2 - Number of people in system at arrival
    3 - Number of people in queue at arrival
    4 - Service times
    5 - Departure times
    */
    int num_class;
    using C_type = std::function<int(float t)>; /*!< Function type float -> int*/
    using event_type = std::function<float(float t)>; /*!< Function type float -> float*/
    using priority_type = std::function<int()>;
    C_type C; /*!< Server update function */
    event_type DepartureTimes; /*!< Service time generator. */
    priority_type Priority_type;/*< Prioirty generator. */

public:
    station(long init_mxN, C_type C_para, event_type dept_para, int init_num_class = 0, priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C(C_para), DepartureTimes(dept_para), Priority_type(priority_type_para)
    {
        mxN = init_mxN;

        server_status.assign(mxN, -1);
        current_customer.assign(mxN, {-1,-1});
        td.assign(mxN, INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers.
        for (int j = 0; j < std::min(n, c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = {Priority_type(),j};
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }

    station(long init_mxN, C_type C_para, float init_dept,int init_num_class = 0, priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C(C_para),
          DepartureTimes([init_dept](float t) -> float { return init_dept; }),
          Priority_type(priority_type_para)
    {
        mxN = init_mxN;

        server_status.assign(mxN, -1);
        current_customer.assign(mxN, {-1,-1});
        td.assign(mxN, INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers.
        for (int j = 0; j < std::min(n, c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = {Priority_type(),j};
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }

    station(long init_mxN, int init_C, float init_dept, int init_num_class = 0, priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C([init_C](float t) -> int { return init_C; }),
          DepartureTimes([init_dept](float t) -> float { return init_dept; }),
          Priority_type(priority_type_para)
    {
        mxN = init_mxN;

        server_status.assign(mxN, -1);
        current_customer.assign(mxN, {-1,-1});
        td.assign(mxN, INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers.
        for (int j = 0; j < std::min(n, c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = {Priority_type(),j};
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }

    station(long init_mxN, int init_C, event_type init_dept,int init_num_class = 0, priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C([init_C](float t) -> int { return init_C; }),
          DepartureTimes(init_dept),
          Priority_type(priority_type_para)
    {
        mxN = init_mxN;

        server_status.assign(mxN, -1);
        current_customer.assign(mxN, {-1,-1});
        td.assign(mxN, INF);
        n = init_n;
        c = 0;
        this->server_updates((t - int(t)) + int(t) % 1440);
        Na = n;
        // Initialising n customers.
        for (int j = 0; j < std::min(n, c); j++)
        {
            server_status[j] = 1;
            td[j] = DepartureTimes(0);
            current_customer[j] = {Priority_type(),j};
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }
    
    void print_station_status(float t);
    int find_min_k();
    float find_min_td();
    void server_updates(float t);
    customer departure_updates(float t);
    void add_customer_to_station(float t,customer curr_customer);
    void write_to_csv(std::string file_name);
    std::vector<customer_data> get_counter_variable();
    int find_queue_len();
    void logger(int station_id, float t);
    void logger(std::string station_id, float t);
    void reset_queue(float t);
    float minimum_residual_time(float t);
    std::tuple<int,int,int> access_system_state(float t); /*!< Server update function */
    void initialize_CSV(std::string file_name);
    void dump_counter_variable_memory(std::string file_name);
    void dump_counter_variable_memory();
    int num_classes();
};


std::vector<float> read_csv(std::string filename,int index);
#endif