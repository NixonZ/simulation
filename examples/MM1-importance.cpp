#include "../components/station.h"

int main()
{
    srand((unsigned)time(NULL));

    float t,ta,temp,mean,variance,X,Y;
    int discrete_events,arriving_customer,customer_departing,i;
    float mean_y,var_y;
    float T = 1;
    float utility = 0;
    std::ofstream fout("./output/MM1-important.csv");
    fout<<"Wait time>x \n"; //,Service time,Controlled estimator\n";

    float U,arr_time,ser_time;

    float total_mean = 0;

    std::tuple<int, float, int, int, float, float> y = {0,0.0,0,0,0.0,0.0};

    for(int k = 0;k<10;k++)
    {
        station MM1(1,1, 
        [T](float t) -> float 
        { 
            float U = random;
            U = log(1+U*(exp(-T)-1))/(-1.0*T);
            return -log( U )/0.12; 
        } );
        discrete_events = 0;
        arriving_customer = 0;
        customer_departing = 0;
        t = 0;
        U = random;
        U = log(1+U*(exp(T)-1))/(T);
        ta = t - log(U)/0.1;
        temp=mean=variance = 0;
        mean_y=var_y=0;
        i = 0;
        utility = 0;
        y = {0,0.0,0,0,0.0,0.0};
        while(discrete_events<1000)
        {
            t = std::min(MM1.find_min_td(), ta);
            MM1.server_updates(t);

            if(t==ta)
            {
                MM1.add_customer_to_station(t,arriving_customer++);
                U = random;
                U = log(1+U*(exp(T)-1))/(T);
                ta = t - log(U)/0.1;
            }
            else
            {
                customer_departing = MM1.departure_updates(t);
                // std::cout<<"Us:" <<Us<<'\n';
                // std::cout<<"Ut:" <<Ut<<'\n';
                for(auto&x: MM1.get_counter_variable())
                {
                    if( std::get<0>(x) == customer_departing-1 && customer_departing!=0 )
                    {   
                        y = x;
                    }
                    if( std::get<0>(x) == customer_departing )
                    {
                        X = std::get<4>(x) - std::get<1>(x);
                        arr_time = std::get<1>(x) - std::get<1>(y);
                        ser_time = std::get<5>(x) - std::get<4>(x);
                        Y = std::get<5>(x) - std::get<4>(x);
                    }
                }

                utility +=   log ( (exp(T)+exp(-T) - 2 ) )- 2*log(T) - T*(exp(-0.1*arr_time) - exp(-0.12*ser_time) );
                
                // Making Indicator Variable
                if(X>50)
                    X=1;
                else
                    X=0;

                X = X;

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
        if(k == 9)
            MM1.write_to_csv("./output/MM1"+std::to_string(k));
        fout<<mean*exp(utility) <<'\n';
        std::cout<<"Mean"<<mean*exp(utility) <<'\n' ;
        std::cout<<"Utility:" <<utility <<'\n';
        std::cout<<k <<'\n';
        total_mean += (mean*exp(utility)-total_mean)/((float)k+1.0);
    }
    std::cout<<"Total Mean:" <<total_mean <<'\n';
    fout.close();
}