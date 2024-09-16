#ifndef _DELTASA_HPP
#define _DELTASA_HPP

#include <memory>

struct DeltaSA
{
    std::unique_ptr<int32_t []> dsa;
    std::unique_ptr<int32_t []> sample;
};

int32_t get_from_dsa(DeltaSA* dsa, int32_t i) {
    return 0;
}

int32_t sa_value(int32_t dsa[], int32_t dsa_sample[], int32_t delta, int32_t index) {
    int32_t r = 0;
    int32_t j = index;

    while (j % delta != 0)
    {
        r += dsa[j--];
    }
    
    return j == 0 ? 
        dsa[j] + r :
        dsa_sample[j / delta - 1] + r;
}

int32_t sa_value_naive(int32_t dsa[], int32_t index) {
    int32_t r = 0;
    int32_t i = index;

    while (i > 0)
    {
        r += dsa[i--];
    }
    
    return dsa[0] + r;
}

#endif