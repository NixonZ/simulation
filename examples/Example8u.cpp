#include"../includes.h"

int bernoullirv(float U,float p)
{
    return 0 + (U>=1-p);
}

float exponential(float U,float lambda)
{
    return -log(U)/lambda;
}

int main()
{  
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_real_distribution<double> distribution (0.0,1.0);
    int n = 20;
    float S = 0;
    float p = 0.4;
    int M = 1000;
    srand((unsigned)time(NULL));

    long double mgf = pow(1.0/1.25,20);
    mgf = pow(3,20);
    float temp = 0;

    long double X = 0;
    long double mean,variance;
    mean = variance = 0;
    int j=0;

    for(int k = 0; k < M ;k++)
    {
        S = 0;
        for (int i = 0; i < n; i++)
        {
            temp = exponential(rnd,1-(-0.25));
            temp = bernoullirv(rnd,0.8);
            S+=temp;
        }
        
        X = 0;

        if(S>=16)
        {
            std::cout<<"````````````Exceed ``````````" <<'\n';
            X = pow(exp(0.25),S)*mgf;
            
            std::cout<<"Iteration number: " <<k <<" Theta:" <<X <<" S:" <<S <<'\n';
        }
        // std::cout<<"Iteration number: " <<k <<" Theta:" <<X <<" S:" <<S <<'\n';
        temp = mean;
        mean = mean + (X-mean)/((float)j+1.0);

        if(j)
            variance = (1-1.0/(float)j)*variance + (j+1)*(mean - temp)*(mean-temp);
        j++;
    }
    std::cout<<"Mean: "<<mean <<'\n' ;
    // std::cout<<"Variance: " <<variance <<'\n';
}