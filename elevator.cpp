#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <tuple>
#include <assert.h>
#define INF 3 * std::numeric_limits<float>::max()
#define random (float)rand() / RAND_MAX;


std::tuple<float,int> min_element(std::vector<float> arr)
{
    int save = 0;
    float min = arr[0];
    for(int i=0;i<arr.size();i++)
    {
        if(arr[i]<min)
        {
            min = arr[i];
            save = i;
        }
    }
    return std::make_tuple(min,save);
}

// next_floor = f(current_floor, floors, elevator)
int algo_next_floor(int current_floor,std::vector<std::deque< std::tuple<int,int,float> >> floors,std::vector<std::tuple<int,int>> elevator,int prev_next_floor,int num_floors,bool stationary)
{

    // implement momentum
    if(prev_next_floor>current_floor)
    {
        int min;
        if(stationary)
            min = current_floor;
        else
            min = current_floor+1;
        if(elevator.size())
        {
            std::vector<int> floors_to_go_to;
            for(auto&x:elevator)
            {
                if( std::get<1>(x)>=min )
                    floors_to_go_to.push_back(std::get<1>(x));
            }
            min = floors_to_go_to[0];
            for(auto&x:floors_to_go_to)
            {
                if( min > x )
                    min = x;
            }
        }
        else
        {
            for(int i = min;i<num_floors;i++)
            {
                if(floors[i].size())
                {
                    min = i;
                    break;
                }
            }
        }
        return min;
    }
    else if(prev_next_floor<current_floor)
    {
        int max;
        if(stationary)
            max = current_floor;
        else
            max = current_floor-1;

        if(elevator.size())
        {
            std::vector<int> floors_to_go_to;
            for(auto&x:elevator)
            {
                if( std::get<1>(x)<= max )
                    floors_to_go_to.push_back(std::get<1>(x));
            }
            max = floors_to_go_to[0];
            for(auto&x:floors_to_go_to)
            {
                if( max < x )
                    max = x;
            }
        }
        else
        {
            for(int i = max;i>=0;i--)
            {
                if(floors[i].size())
                {
                    max = i;
                    break;
                }
            }
        }
        return max;

    }
    // momentum breaker
    else if(prev_next_floor == current_floor)
    {
        // choose the floor which is not empty and has the first customer which arrived.
        float min_time; // min time of arrival
        int save = current_floor;

        // check who all is in the elevator
        if(elevator.size()!=0)
        {
            // go closest to where most poeple want to go
            int min = abs(std::get<1>(elevator[0]) - current_floor);
            std::deque< std::tuple<int,int> > temp; // floor,freq

            for(auto&x : elevator)
            {
                if ( min > abs(std::get<1>(x) - current_floor) )
                {
                    min = abs(std::get<1>(x) - current_floor);
                }
            }

            for(int i =0;i<elevator.size();i++)
            {
                if ( min == abs(std::get<1>(elevator[i]) - current_floor) )
                {
                    bool check = false;
                    for(auto&x:temp)
                    {
                        if( std::get<0>(x) == std::get<1>(elevator[i]) )
                        {
                            std::get<1>(x)++;
                            check = true;
                            break;
                        }
                    }
                    if(!check)
                    {
                        temp.emplace_back( std::make_tuple(std::get<1>(elevator[i]),1) );
                    }
                }
            }
            int max = std::get<1>(temp[0]);
            save = std::get<0>(temp[0]);
            for(auto&x:temp)
            {
                if( max < std::get<1>(x))
                {
                    max = std::get<1>(x);
                    save = std::get<0>(x);
                }
            }

            return save;
        }

        else
        {
            int i = 0;
            while(i<num_floors)
            {
                if(floors[i].size())
                {
                    min_time = std::get<2>(floors[i].front());
                    save = i;
                    break;
                }
                i++;
            }
            if(i == num_floors) // empty system
                return save;
            // first floor not empty
            for(auto&x:floors[i])
            {
                if( min_time > std::get<2>(x) )
                    min_time = std::get<2>(x);
            }

            while(i<num_floors)
            {
                // of all floors
                if(floors[i].size()) // if floor is not empty
                {
                    for(auto&x:floors[i])
                    {
                        if( min_time > std::get<2>(x) ) // find the global min time
                        {
                            min_time = std::get<2>(x);
                            save = i;
                        }
                    }
                }
                i++;
            }
        }
            return save;
        }
}

void print_floors(std::vector<std::deque< std::tuple<int,int,float> >> floors)
{
    int i = 0;
    std::cout<<"----FLOORS----\n";
    for(auto& floor:floors)
    {
        std::cout<<"Floor " <<i++ <<" : " <<"[";
        for(auto& person: floor)
        {
            std::cout<<" ("  <<std::get<0>(person) <<',' <<std::get<1>(person) <<',' <<std::get<2>(person) <<"),";
        }
        std::cout<<"]" <<'\n';
    }
}

void print_elevator(std::vector<std::tuple<int,int>> elevator)
{
    std::cout<<"----ELEVATOR----\n";
    std::cout<<"[";
    for(auto& person:elevator)
    {
        std::cout<<" ("  <<std::get<0>(person) <<',' <<std::get<1>(person) <<"),";
    }
    std::cout<<"]\n";
}

// choose floor = f(current_floor)
int choose_floor(int current_floor,int num_floors)
{
    assert(num_floors>1);
    float U = random;
    float p = 1.0/(num_floors-1);
    for(int i=0;i<num_floors-1;i++)
    {
        if(U<p)
        {
            if(i>=current_floor)
                return i+1;
            else
                return i;
        }
        p = p + p;
    }
}

int main()
{
    srand((unsigned)time(NULL));
    int num_floors = 3;
    std::vector<std::deque< std::tuple<int,int,float> >> floors;
    for(int i =0;i<num_floors;i++)
        floors.push_back(std::deque< std::tuple<int,int,float> >()); //customer no, floor to go to, time of arrival
    std::vector<std::tuple<int,int>> elevator; //customer no, floor to go to
    std::vector<std::tuple<int,int>> elevator_temp;
    elevator.assign(0,{});
    float temp = random;
    std::vector<float> arrival_times(num_floors,0);

    int discrete_events = 0;
    float t = 0;

    float U = INF;
    float D = INF;

    for(auto&x: arrival_times)
    {
        temp = random;
        x = t + -log(temp)*3;
        std::cout<< x <<',';
    }
    std::cout<<'\n';

    int num_customers = 0;

    int next_floor;
    int current_floor = 0;
    bool stationary = true;
    
    next_floor = algo_next_floor(current_floor,floors,elevator,next_floor,num_floors,stationary);

    if(stationary)
    {
        if(next_floor>current_floor)
        {
            U = t + 2.5;
            D = INF;
            stationary = false;
        }
        else if(next_floor<current_floor)
        {
            D = t + 2;
            U = INF;
            stationary = false;
        }
        else
        {
            stationary = true;
            U = INF;
            D = INF;
        }
    }
    
    print_floors(floors);
    print_elevator(elevator);

    while(discrete_events<100)
    {
        std::tuple<float,int> floor_arr = min_element(arrival_times);

        t =  std::min( std::min(U,D), std::get<0>(floor_arr) );

        std::cout<<"------------------------ EVENT " << discrete_events <<" ,t: " <<t
                 <<" ,Min arr time: " <<std::get<0>(floor_arr) <<" ,U: " <<U <<" ,D: " <<D  
                 <<" ,Current floor: " <<current_floor <<" ,Next floor " <<next_floor
                 <<" ,Stationary: " <<stationary
                 <<" ------------------------" <<'\n';

        
        if( t ==  std::get<0>(floor_arr) )
        {
            temp = random;
            arrival_times[std::get<1>(floor_arr)] = t - log(temp)*3;
            floors[std::get<1>(floor_arr)].emplace_back(
                std::make_tuple(num_customers,choose_floor(std::get<1>(floor_arr),num_floors),t)
                );
            std::cout<<"Customer added to floor: " <<std::get<1>(floor_arr) <<" at time " <<t <<'\n';
            num_customers++;

            if(std::get<1>(floor_arr) == current_floor && stationary ) // elevator already present on the same floor we add the customer to elevator
            {  
                elevator_temp.clear();
                for(int i = 0;i<elevator.size();i++)
                {
                    if( std::get<1>(elevator[i]) != current_floor )
                        elevator_temp.push_back(elevator[i]);
                }

                elevator = elevator_temp;

                while(floors[current_floor].size()!=0)
                {
                    elevator.push_back( 
                        std::make_tuple( 
                            std::get<0>( floors[current_floor].front() ),
                            std::get<1>( floors[current_floor].front() ) 
                            ) 
                        );
                    floors[current_floor].pop_front();
                }
            }

            next_floor = algo_next_floor(current_floor,floors,elevator,next_floor,num_floors,stationary);
        }
        else if ( t == U && next_floor>current_floor)
        {
            current_floor++;
            // calc next_floor?
            
            if( next_floor == current_floor )
            {
                stationary = true;
                elevator_temp.clear();
                for(int i = 0;i<elevator.size();i++)
                {
                    if( std::get<1>(elevator[i]) != current_floor )
                        elevator_temp.push_back(elevator[i]);
                }

                elevator = elevator_temp;

                while(floors[current_floor].size()!=0)
                {
                    elevator.push_back( 
                        std::make_tuple( 
                            std::get<0>( floors[current_floor].front() ),
                            std::get<1>( floors[current_floor].front() ) 
                            ) 
                        );
                    floors[current_floor].pop_front();
                }
            }
            next_floor = algo_next_floor(current_floor,floors,elevator,next_floor,num_floors,stationary);
        }
        else if ( t == D && next_floor<current_floor)
        {
            current_floor--;
    
            if( next_floor == current_floor)
            {
                stationary = true;
                elevator_temp.clear();
                for(int i = 0;i<elevator.size();i++)
                {
                    if( std::get<1>(elevator[i]) != current_floor )
                        elevator_temp.push_back(elevator[i]);
                }

                elevator = elevator_temp;

                while(floors[current_floor].size()!=0) // wow
                {
                    elevator.push_back( 
                        std::make_tuple( 
                            std::get<0>( floors[current_floor].front() ),
                            std::get<1>( floors[current_floor].front() ) 
                            ) 
                        );
                    floors[current_floor].pop_front();
                }
            }
            next_floor = algo_next_floor(current_floor,floors,elevator,next_floor,num_floors,stationary);
        }
        else
        {
            //Should not be possible if U= INF and D = inF for stationary elevator
            stationary = true;
            std::cout<<"System Rests for this event\n";
            next_floor = algo_next_floor(current_floor,floors,elevator,next_floor,num_floors,stationary);
        }

        if(stationary)
        {
            if(next_floor>current_floor)
            {
                U = t + 2.5;
                D = INF;
                stationary = false;
            }
            else if(next_floor<current_floor)
            {
                D = t + 2;
                U = INF;
                stationary = false;
            }
            else
            {
                stationary = true;
                U = INF;
                D = INF;
            }
        }
        
        print_floors(floors);
        print_elevator(elevator);

        discrete_events++;
    }

}