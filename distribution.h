#include "includes.h"
class distribution
{
private:
    int b;
    int beta;
    std::vector<float> quantiles;
    float tol;
public:
    distribution(int b_para, int beta_para, std::vector<float> quantiles_para)
    {
        this->b = b_para;
        this->beta = beta_para;
        this->quantiles = quantiles_para;
    }
    ~distribution();
    distribution(event_type sampler)
    {
        
    }
    float sampler(float t)
    {
        float U = random;
        return quantiles[int(b*U)];
    }
};

