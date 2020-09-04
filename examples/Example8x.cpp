#include "../includes.h"
#include<algorithm>

bool sortbysec(const std::tuple<int,float>& a,  
               const std::tuple<int,float>& b) 
{ 
    return (std::get<1>(a) < std::get<1>(b)); 
} 

long int compute_mass(std::vector<int> X)
{
    long int sum = 0;
    for(int i=1;i<=X.size();i++)
        sum += i*X[i-1];
    return sum;
}

int main()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_real_distribution<double> distribution (0.0,1.0);

    std::vector<int> X;
    float theta = 0;
    long int mass;
    float S,P;
    float C = 0;
    for (int i = 1; i <= 100; i++)
        C += log(i);
    C *= 1.7;
    // std::random_device rd;
    // std::mt19937 g(rd());

    // for (size_t i = 1; i <= 100; i++)
    //     X.push_back(i);

    // std::shuffle(X.begin(),X.end(),g);

    // for (size_t i = 1; i <= 100; i++)
    //     std::cout<<X[i-1]<<' ';
    // std::cout<<'\n';

    std::vector<float> lambda;
    for (int j = 1; j <= 100; j++)
        lambda.push_back(-pow(j,-0.7));
    
    std::vector<std::tuple<int,double> > Y;
    for (int j = 1; j <= 100; j++)
        Y.push_back(std::make_tuple(j,rnd*lambda[j-1]));

    sort(Y.begin(), Y.end(), sortbysec); 

    for(auto& elem:Y)
    {
        std::cout<<"("<<std::get<0>(elem) <<"," <<std::get<1>(elem) <<") ";
        X.push_back(std::get<0>(elem));
    }
    std::cout<<"\n";

    for (size_t i = 1; i <= 100; i++)
        std::cout<<X[i-1]<<',';
    std::cout<<'\n';

    int sum = 0;
    for(auto&x: X)
        sum+=x;
    std::cout<<sum <<'\n';
    mass = compute_mass(X);
    std::cout<<mass <<'\n';

    theta = 0;
    if(mass > 290000)
    {
        S = P = 0;
        for(int i = 0;i<100;i++)
        {
            S += pow(X[i],0.7);
            P += log(S);
        }
        theta = exp(P-C);
    }
    std::cout<<theta <<'\n';
}