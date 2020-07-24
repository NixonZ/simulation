#include "components/station.h"
#define random (float)rand() / RAND_MAX;

int main()
{
    srand((unsigned)time(NULL));

    float U = random;
    int N = 10;
    float total_variance = 0;
    float total_mean = 0;
    float Uj,t,ta,temp,mean,variance,X,pj;
    int discrete_events,arriving_customer,customer_departing,i;
    for(int j=1;j<=N;j++)
    {
        // station MM1(1,1, [j,N](float t) -> float { float U = random; float Uj = (U+j-1)/N; return -log(Uj)/0.2; } );
        
        station MM1(1,1, [](float t) -> float { float U = random; return -log(U)/0.1; } );
        U = random;
        Uj = (U+j-1)/N;
        discrete_events = 0;
        arriving_customer = 0;
        t = 0;
        ta = t-log(Uj)/0.1;
        temp=mean=variance = 0;
        i = 0;
        while(discrete_events<1000)
        {
            t = std::min(MM1.find_min_td(), ta);
            MM1.server_updates(t);

            if(t==ta)
            {
                MM1.add_customer_to_station(t,arriving_customer++);
                U = random;
                // Uj = (U+j-1)/N;
                ta = t - log(U)/0.1;
            }
            else
            {
                customer_departing = MM1.departure_updates(t);
                for(auto&x: MM1.get_counter_variable())
                {
                    if( std::get<0>(x) == customer_departing )
                        X = std::get<4>(x) - std::get<1>(x);
                    float control = std::get<4>(x);
                }   
                // Making Indicator Variable
                if(X>120)
                    X=1;
                else
                    X=0;
                temp = mean;
                mean = mean + (X-mean)/(i+1);
                if(i)
                    variance = (1-1.0/(float)i)*variance + (i+1)*(mean - temp)*(mean-temp);
                i++;
            }
            // MM1.logger(std::string("MM1"),t);
            discrete_events++;
        }
        pj = 1.0/N;
        std::cout<<j <<'\n';
        std::cout<<"Mean:" <<mean <<'\n' <<"Variance:" <<variance <<'\n' <<"pj:"<<pj<<'\n';
        total_variance += variance*pj*pj;
        total_mean += mean*pj;
        // MM1.write_to_csv(std::string("./output/MM1"+std::to_string(j)));
        // MM1.reset_queue();
    }
    std::cout<<"\nTotal mean:" <<total_mean <<'\n';
    std::cout<<"\nTotal Variance:" <<total_variance <<'\n';
}