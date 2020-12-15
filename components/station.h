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
    C_type C; /*!< Server update function */ 
    event_type_list DepartureTimes; /*!< Service time generator for each priority . */
    priority_type Priority_type; /*< Prioirty generator. */
    event_type_list PatienceTimes; /*< Patience time generator for each prioirity. */

public:

/// @brief Constructor for Station Class
///
///  @param init_mxN  Max number of servers.
///  @param C_para Function returning number of active servers with time. Dynamic Servers.
///  @param dept_para A vector of Service time generators for each priority level.
///  @param PatienceTimes_para A vector of Patience time generators for each priority level. Leave Empty if and only if there is a single class.
///  @param init_num_class Number of prioirity levels
///  @param priority_type_gen The function which randamly generates the prioiry types. Can be ignored only in case of a single class system
///  @param t initial time of the system
///  @param init_n Number of customers present at t=0
///
///  @details
///  Dynamic servers with
///  Each priority customer having different service pattern.
    station(long init_mxN, C_type C_para, event_type_list dept_para, int init_num_class = 0, event_type_list PatienceTimes_para = event_type_list( 1 , [](float t) -> float { return (0.0); }  ), priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C(C_para), DepartureTimes(dept_para), Priority_type(priority_type_para), PatienceTimes(PatienceTimes_para)
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
            int priority = Priority_type();
            current_customer[j] = {priority,j};
            td[j] = DepartureTimes[priority](0);
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }

/// @brief Constructor for Station Class
///
///  @param init_mxN  Max number of servers.
///  @param C_para Function returning number of active servers with time. Dynamic Servers.
///  @param dept_para A vector of Service time generators for each priority level.
///  @param PatienceTimes_para Deterministic service time for each prioirty customer.
///  @param init_num_class Number of prioirity levels
///  @param priority_type_gen Discrete Patience times for each priority level.
///  @param t initial time of the system
///  @param init_n Number of customers present at t=0
///
///  @details
///  Dynamic servers with
///  Each priority customer having different service pattern.
    station(long init_mxN, C_type C_para, event_type_list dept_para, int init_num_class = 0, float PatienceTimes_para = 0, priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C(C_para), DepartureTimes(dept_para), Priority_type(priority_type_para), 
            PatienceTimes(event_type_list( init_num_class , [PatienceTimes_para](float t) -> float { return PatienceTimes_para; }  ))
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
            int priority = Priority_type();
            current_customer[j] = {priority,j};
            td[j] = DepartureTimes[priority](0);
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }

/// @brief Constructor for Station Class
///
///  @param init_mxN  Max number of servers.
///  @param C_para Function returning number of active servers with time. Dynamic Servers.
///  @param init_dept Deterministic service time for each prioirty customer.
///  @param init_num_class Number of prioirity levels
///  @param priority_type_gen The function which randamly generates the prioiry types. Can be ignored only in case of a single class system
///  @param t initial time of the system
///  @param init_n Number of customers present at t=0
///
///  @details
///  Dynamic servers with
///  Each priority customer having deterministic Service Time.
    station(long init_mxN, C_type C_para, float init_dept, int init_num_class = 0, event_type_list PatienceTimes_para = event_type_list( 1 , [](float t) -> float { return (0.0); }  ), priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C(C_para),
          DepartureTimes( event_type_list( init_num_class , [init_dept](float t) -> float { return init_dept; }  ) ),
          Priority_type(priority_type_para),PatienceTimes(PatienceTimes_para)
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
            int priority = Priority_type();
            current_customer[j] = {priority,j};
            td[j] = DepartureTimes[priority](0);
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }

/// @brief Constructor for Station Class
///
///  @param init_mxN  Max number of servers
///  @param init_C Number of servers
///  @param init_dept Deterministic service time for each prioirty customer
///  @param init_num_class Number of prioirity levels
///  @param priority_type_gen The function which randamly generates the prioiry types. Can be ignored only in case of a single class system
///  @param t initial time of the system
///  @param init_n Number of customers present at t=0
///
///  @details
///  Constant number of Servers (static servers) with
///  Each priority customer having deterministic Service Time.  
    station(long init_mxN, int init_C, float init_dept, int init_num_class = 0, event_type_list PatienceTimes_para = event_type_list( 1 , [](float t) -> float { return (0.0); }  ) , priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C([init_C](float t) -> int { return init_C; }),
          DepartureTimes( event_type_list( init_num_class, [init_dept](float t) -> float { return init_dept; } ) ),
          Priority_type(priority_type_para),PatienceTimes(PatienceTimes_para)
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
            int priority = Priority_type();
            current_customer[j] = {priority,j};
            td[j] = DepartureTimes[priority](0);
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }

/// @brief Constructor for Station Class
///
///  @param init_mxN  Max number of servers.
///  @param init_C Number of servers.
///  @param dept_para A vector of Service time generators for each priority level.
///  @param init_num_class Number of prioirity levels
///  @param priority_type_gen The function which randamly generates the prioiry types. Can be ignored only in case of a single class system
///  @param t initial time of the system
///  @param init_n Number of customers present at t=0
///
///  @details
///  Constant number of Servers (static servers) with
///  Each priority customer having different service pattern.
    station(long init_mxN, int init_C, event_type_list dept_para, int init_num_class = 0,  event_type_list PatienceTimes_para = event_type_list( 1 , [](float t) -> float { return (0.0); }  ), priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C([init_C](float t) -> int { return init_C; }),
          DepartureTimes(dept_para),
          Priority_type(priority_type_para),
          PatienceTimes(PatienceTimes_para)
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
            int priority = Priority_type();
            current_customer[j] = {priority,j};
            td[j] = DepartureTimes[priority](0);
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }


/// @brief Constructor for Station Class
///
///  @param init_mxN  Max number of servers.
///  @param init_C Number of servers.
///  @param dept_para A vector of Service time generators for each priority level.
///  @param init_num_class Number of prioirity levels
///  @param priority_type_gen The function which randamly generates the prioiry types. Can be ignored only in case of a single class system
///  @param t initial time of the system
///  @param init_n Number of customers present at t=0
///
///  @details
///  Constant number of Servers (static servers) with
///  Each priority customer having same service pattern.  
    station(long init_mxN, int init_C, event_type init_dept, int init_num_class = 0, event_type_list PatienceTimes_para = event_type_list( 1 , [](float t) -> float { return (0.0); }  ), priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C([init_C](float t) -> int { return init_C; }),
          DepartureTimes( event_type_list(init_num_class,init_dept) ),
          Priority_type(priority_type_para),
          PatienceTimes(PatienceTimes_para)
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
            int priority = Priority_type();
            current_customer[j] = {priority,j};
            td[j] = DepartureTimes[priority](0);
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }
/// @brief Constructor for Station Class
///
///  @param init_mxN  Max number of servers.
///  @param init_C Number of servers.
///  @param dept_para A vector of Service time generators for each priority level.
///  @param init_num_class Number of prioirity levels
///  @param priority_type_gen The function which randamly generates the prioiry types. Can be ignored only in case of a single class system
///  @param t initial time of the system
///  @param init_n Number of customers present at t=0
///
///  @details
///  Dynamic servers with
///  Each priority customer having same service pattern.  
    station(long init_mxN, C_type C_para, event_type init_dept,int init_num_class = 0,event_type_list PatienceTimes_para = event_type_list( 1 , [](float t) -> float { return (0.0); }  ) , priority_type priority_type_para = []() -> int {return 0;}, float t = 0, int init_n = 0)
        : C(C_para),
          DepartureTimes( event_type_list(init_num_class,init_dept) ),
          Priority_type(priority_type_para),
          PatienceTimes(PatienceTimes_para)
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
            int priority = Priority_type();
            current_customer[j] = {priority,j};
            td[j] = DepartureTimes[priority](0);
        }
        for (int i = std::min(n, c); i < n; i++)
        {
            current_queue.insert({Priority_type(),i});
        }
        num_class = init_num_class;
    }    
    
/// Copy Constructor
    station(const station&copy)
    {
        this->mxN = copy.mxN;
        this->server_status = copy.server_status;
        this->current_customer = copy.current_customer;
        this->current_queue = copy.current_queue;
        this->td = copy.td;
        this->c = copy.c;
        this->n = copy.n;
        this->Na = copy.Na;
        this->counter_variable = copy.counter_variable;
        this->num_class = copy.num_class;
        this->C = copy.C;
        this->DepartureTimes = copy.DepartureTimes;
        this->Priority_type = copy.Priority_type;
        this->PatienceTimes = copy.PatienceTimes;
    }

    void print_station_status(float t);
    int find_min_k();
    float find_min_td();
    void server_updates(float t);
    customer departure_updates(float t);
    void add_customer_to_station(float t,customer curr_customer,bool keep_virtual = false, event_type_list arrival_processes = { [](float t)-> float{return 0;} },std::vector<float> ta = {0.0});
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
std::vector<float> call_event_type_list(event_type_list );
#endif