#include"../components/station.h"

int main()
{
    srand((unsigned)time(NULL));
    float mu = 0.1;
    float lambda = 0.05;
    station temp(2,2,
    [mu](float t) -> float
    {
        return -log(rnd)/mu;
    });
    int discrete_events,arriving_customer,customer_departing,i;
    discrete_events=arriving_customer=customer_departing=i=0;
    float t,ta;
    t=0;
    ta = t - log(rnd)/lambda;
    temp.print_station_status(t);
    while(discrete_events<3)
    {
        t = std::min(temp.find_min_td(),ta);
        if(t==ta)
        {
            temp.add_customer_to_station(t,arriving_customer++);
            ta = t - log(rnd)/lambda;
        }
        else
        {
            customer_departing = temp.departure_updates(t);

        }
        temp.print_station_status(t);
        discrete_events++;
    }
    temp.reset_queue(t);
    temp.print_station_status(t);
}