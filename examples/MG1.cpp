#include "components/station.h"
#define random (float)rand() / RAND_MAX;
#define rnd  ( (float)rand() / RAND_MAX )

int main()
{
    srand((unsigned)time(NULL));
    std::vector<float> service_times = read_csv("lognormal.csv",1);
    std::vector<float> interarrivaltimes = read_csv("lognormal.csv",2);
    
    station MG1(1,1,
    [service_times](float t) -> float
    {
        float U = random;
        int index = (int)(U*service_times.size());
        try
        {
            return service_times[index];
        }
        catch(const std::exception& e)
        {
            // std::cerr << e.what() << '\n';
            return service_times[index-1];
        }
    });

    float t,ta,temp,mean,variance,X,pj,control;
    int discrete_events,arriving_customer,customer_departing,i;

    ta = interarrivaltimes[(int)(rnd*interarrivaltimes.size())];
    t=temp=mean=variance=X=pj=0;
    discrete_events=arriving_customer=customer_departing=i=0;

    while(discrete_events<1000)
    {
        t = std::min(MG1.find_min_td(),ta);
        MG1.server_updates(t);

        if( t==ta )
        {
            MG1.add_customer_to_station(t,arriving_customer++);
            ta = interarrivaltimes[(int)(rnd*interarrivaltimes.size())];
        }
        else
        {
            customer_departing = MG1.departure_updates(t);
            for(auto&x: MG1.get_counter_variable())
            {
                if( std::get<0>(x) == customer_departing )
                {
                    X = std::get<4>(x) - std::get<1>(x);
                    control = std::get<4>(x) - std::get<4>(MG1.get_counter_variable()[customer_departing-1]);
                }
            }  
            X = control*X;
            // Making Indicator Variable
            // if(X>120)
            //     X=1;
            // else
            //     X=0;
            temp = mean;
            mean = mean + (X-mean)/(i+1);
            if(i)
                variance = (1-1.0/(float)i)*variance + (i+1)*(mean - temp)*(mean-temp);
            i++;
        }
        discrete_events++;
    }
    std::cout<<"Mean:" <<mean <<'\n' <<"Variance:" <<variance <<'\n';
    // MG1.write_to_csv("./output/MG1");
}