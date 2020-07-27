#include "../components/station.h"
#define random (float)rand() / RAND_MAX;

int main()
{
    srand((unsigned)time(NULL));

    float U = random;
    float t,ta,temp,mean,variance,X,pj,Y;
    int discrete_events,arriving_customer,customer_departing,i;
    float mean_y,var_y;
    std::ofstream fout("./output/MM1-controlled.csv");
    fout<<"Wait time,Service time,Controlled estimator\n";
    for(int k = 0;k<100;k++)
    {
        station MM1(1,1, [](float t) -> float { float U = random; return -log(U)/0.12; } );
        U = random;
        discrete_events = 0;
        arriving_customer = 0;
        t = 0;
        ta = t-log(U)/0.1;
        temp=mean=variance = 0;
        mean_y=var_y=0;
        i = 0;
        while(discrete_events<10000)
        {
            t = std::min(MM1.find_min_td(), ta);
            MM1.server_updates(t);

            if(t==ta)
            {
                MM1.add_customer_to_station(t,arriving_customer++);
                U = random;
                ta = t - log(U)/0.1;
            }
            else
            {
                customer_departing = MM1.departure_updates(t);
                for(auto&x: MM1.get_counter_variable())
                {
                    if( std::get<0>(x) == customer_departing )
                    {
                        X = std::get<4>(x) - std::get<1>(x);
                        Y = std::get<4>(x) - std::get<4>(MM1.get_counter_variable()[customer_departing-1]);
                    }
                }
                temp = mean;
                mean = mean + (X-mean)/((float)i+1.0);
                if(i)
                    variance = (1-1.0/(float)i)*variance + (i+1)*(mean - temp)*(mean-temp);

                temp = mean_y;
                mean_y = mean_y + (Y-mean_y)/((float)i+1.0);
                if(i)
                    var_y = (1-1.0/(float)i)*var_y + (i+1)*(mean_y - temp)*(mean_y-temp);

                i++;
            }
            discrete_events++;
        }
        fout<<mean <<',' <<mean_y <<','<<mean + (-(-4.938652/0.213848))*(mean_y-1/0.12) <<'\n';
        std::cout<<mean <<',' <<mean_y <<','<<mean + (-(-4.938652/0.213848))*(mean_y-1/0.12) <<'\n';
        std::cout<<k <<'\n';
    }
    fout.close();
    // MM1.reset_queue();
}